#include "parser.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/token.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "types/type.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace frontend {

Parser::Parser(Lexer &lex, ErrorReporter &errors) 
	: lexer(lex), current(lexer.get()), errors(errors) {}

void Parser::advance() { current = lexer.get(); }

bool Parser::match(TokenType type) { return current.type == type; }

bool Parser::isType(TokenType type) {
	return type == TokenType::INT ||
	       type == TokenType::FLOAT ||
	       type == TokenType::DOUBLE ||
	       type == TokenType::BOOL ||
	       type == TokenType::CHAR ||
	       type == TokenType::STRING ||
	       type == TokenType::VOID ||
	       type == TokenType::INFER;
}

bool Parser::isLiteral(TokenType type) {
	return type == TokenType::NUMBER ||
		type == TokenType::STRING_LIT ||
		type == TokenType::CHAR_LIT ||
		type == TokenType::TRUE ||
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


std::optional<std::unique_ptr<ast::ASTNode>> Parser::parseLiteral() {
	switch (current.type) {
		case TokenType::NUMBER: {
			auto result = std::make_unique<ast::NumberLiteralAST>(std::stod(current.lexeme));
			advance();
			return result;
		}
		case TokenType::STRING_LIT: {
			auto result = std::make_unique<ast::StringLiteralAST>(std::move(current.lexeme));
			advance();
			return result;
		}
		case TokenType::CHAR_LIT: {
			std::string temp = current.lexeme;
			const char *value = temp.c_str();
			auto result = std::make_unique<ast::CharLiteralAST>(*value);
			advance();
			return result;
		}
		case TokenType::TRUE: {
			auto result = std::make_unique<ast::BoolLiteralAST>(false);
			advance();
			return result;
		}
		case TokenType::FALSE: {
			auto result = std::make_unique<ast::BoolLiteralAST>(false);
			advance();
			return result;
		}
		default:
			errors.error(
                	"Expected literal but got " + current.lexeme,
                	current.line,
                	current.column
            		);
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
			errors.error(
                	"Expected primitive type but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return std::nullopt;
	}
}

std::optional<std::unique_ptr<types::Type>> Parser::parseType() {
	switch (current.type) {
		case TokenType::CONST: {
			advance();
			if (auto inner_type = parseType(); inner_type) { 
				return std::make_unique<types::ConstType>(std::move(*inner_type));
			}
			errors.error(
                	"Expected type after 'const' but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return std::nullopt;
		}
		case TokenType::STATIC: {
			advance();
			if (auto inner_type = parseType(); inner_type) {
				return std::make_unique<types::StaticType>(std::move(*inner_type));
			}
			errors.error(
                	"Expected type after 'static' but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return std::nullopt;
		}
		case TokenType::STRUCT: {
			advance();
			if (current.type == TokenType::IDENT) {
				std::string struct_name = current.lexeme;
				advance();
				return std::make_unique<types::StructType>(std::move(struct_name));
			}
			errors.error(
                	"Expected identifier after 'struct' but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return std::nullopt;
		}
		default:
			return parsePrimitiveType();
	}
}

std::optional<std::unique_ptr<ast::ASTNode>> Parser::parsePrimaryExpr() {
	if (current.type == TokenType::IDENT) {
		auto name = std::make_unique<ast::VariableExprAST>(std::move(current.lexeme));
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
			errors.error(
			"Expected '}' but got " + current.lexeme,
			current.line,
			current.column
			);
			return std::nullopt;
		}
	}
	errors.error(
	"Expected identifier, literal or '(' but got " + current.lexeme,
	current.line,
	current.column
	);
	return std::nullopt;
}

std::vector<std::unique_ptr<ast::ExprAST>> Parser::parseArgListTail(std::unique_ptr<ast::ExprAST> expr) {
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

std::optional<std::unique_ptr<ast::ExprAST>> Parser::parseExpression() {
	return std::nullopt;
}

std::optional<std::unique_ptr<ast::StmtAST>> Parser::parseVarDecl() { return std::nullopt; }

}
