#include "parser.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace frontend {

Parser::Parser(Lexer &lex, ErrorReporter &errors)
    : lexer(lex), current(lexer.get()), errors(errors) {}

void Parser::advance() { current = lexer.get(); }

bool Parser::match(TokenType type) { return current.type == type; }

bool Parser::isType(TokenType type) {
	return type == TokenType::INT || type == TokenType::FLOAT ||
	       type == TokenType::DOUBLE || type == TokenType::BOOL ||
	       type == TokenType::CHAR || type == TokenType::STRING ||
	       type == TokenType::VOID || type == TokenType::INFER;
}

bool Parser::isLiteral(TokenType type) {
	return type == TokenType::NUMBER || type == TokenType::STRING_LIT ||
	       type == TokenType::CHAR_LIT || type == TokenType::TRUE ||
	       type == TokenType::FALSE;
}

bool Parser::expect(TokenType type) {
	if (!match(type)) {
		std::cerr << "Expected " << static_cast<int>(type)
			  << " but got " << static_cast<int>(current.type)
			  << "\n";
		return false;
	}
	return true;
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parseLiteral() {
	switch (current.type) {
	case TokenType::NUMBER: {
		auto result = std::make_unique<ast::NumberLiteralAST>(
		    std::stod(current.lexeme));
		advance();
		return result;
	}
	case TokenType::STRING_LIT: {
		auto result = std::make_unique<ast::StringLiteralAST>(
		    std::move(current.lexeme));
		advance();
		return result;
	}
	case TokenType::CHAR_LIT: {
		std::string temp = current.lexeme;
		char value = temp[1];
		auto result = std::make_unique<ast::CharLiteralAST>(value);
		advance();
		return result;
	}
	case TokenType::TRUE: {
		auto result = std::make_unique<ast::BoolLiteralAST>(true);
		advance();
		return result;
	}
	case TokenType::FALSE: {
		auto result = std::make_unique<ast::BoolLiteralAST>(false);
		advance();
		return result;
	}
	default:
		errors.error("Expected literal but got " + current.lexeme,
			     current.line, current.column);
		return std::nullopt;
	}
}

bool Parser::unaryOperator() const {
	return current.type == TokenType::BIT_AND ||
	       current.type == TokenType::MULTIPLY ||
	       current.type == TokenType::PLUS ||
	       current.type == TokenType::MINUS ||
	       current.type == TokenType::BIT_NOT ||
	       current.type == TokenType::NOT;
}

bool Parser::assignmentOperator() const {
	return current.type == TokenType::EQUAL ||
	       current.type == TokenType::MULTIPLY_ASSIGN ||
	       current.type == TokenType::DIVIDE_ASSIGN ||
	       current.type == TokenType::MODULO_ASSIGN ||
	       current.type == TokenType::PLUS_ASSIGN ||
	       current.type == TokenType::MINUS_ASSIGN ||
	       current.type == TokenType::LEFT_SHIFT_ASSIGN ||
	       current.type == TokenType::RIGHT_SHIFT_ASSIGN ||
	       current.type == TokenType::BIT_AND_ASSIGN ||
	       current.type == TokenType::BIT_XOR_ASSIGN ||
	       current.type == TokenType::BIT_OR_ASSIGN;
}

std::optional<std::unique_ptr<types::Type>> Parser::parsePrimitiveType() {
	switch (current.type) {
	case TokenType::INT: {
		advance();
		return std::make_unique<types::IntType>();
	}
	case TokenType::STRING: {
		advance();
		return std::make_unique<types::StringType>();
	}
	case TokenType::CHAR: {
		advance();
		return std::make_unique<types::CharType>();
	}
	case TokenType::BOOL: {
		advance();
		return std::make_unique<types::BoolType>();
	}
	case TokenType::FLOAT: {
		advance();
		return std::make_unique<types::FloatType>();
	}
	case TokenType::DOUBLE: {
		advance();
		return std::make_unique<types::DoubleType>();
	}
	case TokenType::VOID: {
		advance();
		return std::make_unique<types::VoidType>();
	}
	default:
		errors.error("Expected primitive type but got " +
				 current.lexeme,
			     current.line, current.column);
		return std::nullopt;
	}
}

std::optional<std::unique_ptr<types::Type>> Parser::parseType() {
	switch (current.type) {
	case TokenType::CONST: {
		advance();
		if (auto inner_type = parseType(); inner_type) {
			return std::make_unique<types::ConstType>(
			    std::move(*inner_type));
		}
		errors.error("Expected type after 'const' but got " +
				 current.lexeme,
			     current.line, current.column);
		return std::nullopt;
	}
	case TokenType::STATIC: {
		advance();
		if (auto inner_type = parseType(); inner_type) {
			return std::make_unique<types::StaticType>(
			    std::move(*inner_type));
		}
		errors.error("Expected type after 'static' but got " +
				 current.lexeme,
			     current.line, current.column);
		return std::nullopt;
	}
	case TokenType::STRUCT: {
		advance();
		if (current.type == TokenType::IDENT) {
			std::string struct_name = current.lexeme;
			advance();
			return std::make_unique<types::StructType>(
			    std::move(struct_name));
		}
		errors.error("Expected identifier after 'struct' but got " +
				 current.lexeme,
			     current.line, current.column);
		return std::nullopt;
	}
	default:
		return parsePrimitiveType();
	}
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parsePrimaryExpr() {
	if (current.type == TokenType::IDENT) {
		auto name = std::make_unique<ast::VariableExprAST>(
		    std::move(current.lexeme));
		advance();
		return name;
	}
	if (auto lit = parseLiteral(); lit) {
		return lit;
	}
	if (current.type == TokenType::OPAREN) {
		advance();
		if (auto expr = parseExpression(); expr) {
			if (current.type == TokenType::CPAREN) {
				advance();
				return expr;
			}
			errors.error("Expected '}' but got " + current.lexeme,
				     current.line, current.column);
			return std::nullopt;
		}
	}
	errors.error("Expected identifier, literal or '(' but got " +
			 current.lexeme,
		     current.line, current.column);
	return std::nullopt;
}

std::vector<std::unique_ptr<ast::ExprAST>>
Parser::parseArgListTail(std::unique_ptr<ast::ExprAST> expr) {
	std::vector<std::unique_ptr<ast::ExprAST>> args;
	args.emplace_back(std::move(expr));

	while (current.type == TokenType::COMMA) {
		advance();
		auto next_expr = parseExpression();
		if (next_expr) {
			args.emplace_back(std::move(*next_expr));
		}
		else {
			return args;
		}
	}
	if (current.type != TokenType::CPAREN) {
		errors.error("Expected ')' but got " + current.lexeme,
			     current.line, current.column);
		return args;
	}
	return args;
}

std::vector<std::unique_ptr<ast::ExprAST>> Parser::parseArgList() {
	auto expr = parseExpression();
	if (expr) {
		auto arg_list_t = parseArgListTail(std::move(*expr));
		return arg_list_t;
	}
	return {};
}

std::optional<std::unique_ptr<ast::ExprAST>>
Parser::parsePostfixExprTail(std::unique_ptr<ast::ExprAST> primary_expr) {
	if (current.type == TokenType::LSQRBRACKET) {
		advance();
		auto expr = parseExpression();
		if (expr) {
			if (current.type == TokenType::RSQRBRACKET) {
				return expr;
			}
			errors.error("Expected ']' but got " + current.lexeme,
				     current.line, current.column);
			return std::nullopt;
		}
		return std::nullopt;
	}
	if (current.type == TokenType::OPAREN) {
		advance();

		auto args = parseArgList();
		if (current.type == TokenType::CPAREN) {
			advance();
			return std::make_unique<ast::CallExprAST>(
			    std::move(primary_expr), std::move(args));
		}
	}
	if (current.type == TokenType::DOT) {
		advance();
		if (current.type == TokenType::IDENT) {
			auto name = std::move(current.lexeme);
			advance();
			return std::make_unique<ast::VariableExprAST>(
			    std::move(name));
		}
	}
	if (current.type == TokenType::SCOPE) {
		advance();
		if (current.type == TokenType::IDENT) {
			auto name = std::move(current.lexeme);
			advance();
			return std::make_unique<ast::VariableExprAST>(
			    std::move(name));
		}
	}
	if (current.type == TokenType::PLUS_PLUS) {
		advance();
		return std::make_unique<ast::UnaryExprAST>(
		    ast::UnaryOp::POST_INCREMENT, std::move(primary_expr));
	}
	if (current.type == TokenType::MINUS_MINUS) {
		advance();
		return std::make_unique<ast::UnaryExprAST>(
		    ast::UnaryOp::POST_DECREMENT, std::move(primary_expr));
	}
	switch (current.type) {
	case TokenType::MULTIPLY:
	case TokenType::DIVIDE:
	case TokenType::MODULO:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::LEFT_SHIFT:
	case TokenType::RIGHT_SHIFT:
	case TokenType::LESS:
	case TokenType::GREATER:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER_EQUAL:
	case TokenType::EQUAL:
	case TokenType::NOT_EQUAL:
	case TokenType::BIT_AND:
	case TokenType::BIT_XOR:
	case TokenType::BIT_OR:
	case TokenType::AND:
	case TokenType::OR:
	case TokenType::QUESTION:
	case TokenType::COMMA:
	case TokenType::RSQRBRACKET:
	case TokenType::CPAREN:
	case TokenType::SEMICOLON:
		return primary_expr;
	default:
		errors.error(
		    "Expected '[', '(', '.', '::', '++', or '--' but got " +
			current.lexeme,
		    current.line, current.column);
		return std::nullopt;
	}
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parsePostfixExpr() {
	std::optional<std::unique_ptr<ast::ExprAST>> lhsOpt =
	    parsePrimaryExpr();
	if (!lhsOpt) {
		return std::nullopt;
	}

	std::unique_ptr<ast::ExprAST> lhs = std::move(*lhsOpt);
	return parsePostfixExprTail(std::move(lhs));
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parseUnaryExpr() {
	if (auto is_unary_op = unaryOperator(); is_unary_op) {
		ast::UnaryOp unary_op{};

		switch (current.type) {
		case TokenType::AND:
			unary_op = ast::UnaryOp::AND;
			break;
		case TokenType::MULTIPLY:
			unary_op = ast::UnaryOp::MUL;
			break;
		case TokenType::PLUS:
			unary_op = ast::UnaryOp::PLUS;
			break;
		case TokenType::MINUS:
			unary_op = ast::UnaryOp::MINUS;
			break;
		case TokenType::NOT:
			unary_op = ast::UnaryOp::NOT;
			break;
		case TokenType::BIT_NOT:
			unary_op = ast::UnaryOp::BIT_NOT;
			break;
		default:
			errors.error("Invalid unary operator", current.line,
				     current.column);
			return std::nullopt;
		}
		advance();

		if (auto postfix_expr = parsePostfixExpr()) {
			return std::make_unique<ast::UnaryExprAST>(
			    unary_op, std::move(*postfix_expr));
		}
		return std::nullopt;
	}
	return parsePostfixExpr();
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parseMultiplicativeExpr() {
	auto lhs = parseUnaryExpr();
	if (!lhs) {
		return std::nullopt;
	}

	while (current.type == TokenType::MULTIPLY ||
	       current.type == TokenType::DIVIDE ||
	       current.type == TokenType::MODULO) {

		ast::BinaryOp op{};
		if (current.type == TokenType::MULTIPLY) {
			op = ast::BinaryOp::MUL;
		}
		else if (current.type == TokenType::DIVIDE) {
			op = ast::BinaryOp::DIV;
		}
		else {
			op = ast::BinaryOp::MOD;
		}
		advance();

		auto rhs = parseUnaryExpr();
		if (!rhs) {
			return std::nullopt;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(*lhs),
							   std::move(*rhs));
	}
	return lhs;
}

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parseExpression() {
	return std::nullopt;
}

std::optional<std::unique_ptr<ast::StmtAST>> Parser::parseVarDecl() {
	return std::nullopt;
}

} // namespace frontend
