#include "parser.hpp"
#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <cassert>
#include <cstddef>
#include <execution>
#include <iostream>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace frontend {

Parser::Parser(Lexer &lex, ErrorReporter &errors)
    : lexer(lex), current(lexer.get()), errors(errors) {}

void Parser::advance() { current = lexer.get(); }

bool Parser::expect(TokenType type) {
	if (type != current.type) {
		std::cerr << "Expected " << static_cast<int>(type)
			  << " but got " << static_cast<int>(current.type)
			  << "\n";
		return false;
	}
	return true;
}

std::unique_ptr<ast::ExprAST> Parser::parseLiteral() {
	switch (current.type) {
	case TokenType::NUMBER_LIT: {
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
		return nullptr;
	}
}

bool Parser::unaryOperator() const {
	return current.type == TokenType::AMPERSAND ||
	       current.type == TokenType::STAR ||
	       current.type == TokenType::PLUS ||
	       current.type == TokenType::MINUS ||
	       current.type == TokenType::TILDE ||
	       current.type == TokenType::EXCLAMATION;
}

bool Parser::assignmentOperator() const {
	return current.type == TokenType::EQUAL ||
	       current.type == TokenType::STAR_EQUAL ||
	       current.type == TokenType::SLASH_EQUAL ||
	       current.type == TokenType::PERCENT_EQUAL ||
	       current.type == TokenType::PLUS_EQUAL ||
	       current.type == TokenType::MINUS_EQUAL ||
	       current.type == TokenType::LESS_LESS_EQUAL ||
	       current.type == TokenType::GREATER_GREATER_EQUAL ||
	       current.type == TokenType::AMPERSAND_EQUAL ||
	       current.type == TokenType::CARET_EQUAL ||
	       current.type == TokenType::PIPE_EQUAL;
}

std::unique_ptr<types::Type> Parser::parsePrimitiveType() {
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
		return nullptr;
	}
}

std::unique_ptr<types::Type> Parser::parseType() {
	switch (current.type) {
	case TokenType::CONST: {
		advance();
		if (auto inner_type = parseType(); inner_type) {
			return std::make_unique<types::ConstType>(
			    std::move(inner_type));
		}
		errors.error("Expected type after 'const' but got " +
				 current.lexeme,
			     current.line, current.column);
		return nullptr;
	}
	case TokenType::STATIC: {
		advance();
		if (auto inner_type = parseType(); inner_type) {
			return std::make_unique<types::StaticType>(
			    std::move(inner_type));
		}
		errors.error("Expected type after 'static' but got " +
				 current.lexeme,
			     current.line, current.column);
		return nullptr;
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
		return nullptr;
	}
	default:
		return parsePrimitiveType();
	}
}

// Parses `IDENT (:: IDENT)*`.
std::optional<ast::QualifiedName> Parser::parseQualifiedName() {
	assert(current.type == TokenType::IDENT);
	ast::QualifiedName name;
	name.name = std::move(current.lexeme);
	advance();

	while (current.type == TokenType::COLON_COLON) {
		advance();
		if (current.type != TokenType::IDENT) {
			errors.error("Expected identifier after '::' but got: " +
					 current.lexeme,
				     current.line, current.column);
			return std::nullopt;
		}
		name.qualifiers.push_back(std::move(name.name));
		name.name = std::move(current.lexeme);
		advance();
	}
	return name;
}

std::unique_ptr<ast::ExprAST> Parser::parsePrimaryExpr() {
	if (current.type == TokenType::IDENT) {
		auto name = parseQualifiedName();
		if (!name) {
			return nullptr;
		}
		return std::make_unique<ast::VariableExprAST>(
		    std::move(*name));
	}
	if (current.type == TokenType::LPAREN) {
		advance();
		if (auto expr = parseExpression(); expr) {
			if (current.type == TokenType::RPAREN) {
				advance();
				return expr;
			}
			errors.error("Expected ')' but got " + current.lexeme,
				     current.line, current.column);
			return nullptr;
		}
		return nullptr;
	}
	if (auto lit = parseLiteral(); lit) {
		return lit;
	}
	errors.error("Expected identifier, literal or '(' but got " +
			 current.lexeme,
		     current.line, current.column);
	return nullptr;
}

std::vector<std::unique_ptr<ast::ExprAST>>
Parser::parseArgListTail(std::unique_ptr<ast::ExprAST> expr) {
	std::vector<std::unique_ptr<ast::ExprAST>> args;
	args.emplace_back(std::move(expr));

	while (current.type == TokenType::COMMA) {
		advance();
		auto next_expr = parseExpression();
		if (next_expr) {
			args.emplace_back(std::move(next_expr));
		}
		else {
			return args;
		}
	}
	if (current.type != TokenType::RPAREN) {
		errors.error("Expected ')' but got " + current.lexeme,
			     current.line, current.column);
		return args;
	}
	return args;
}

std::vector<std::unique_ptr<ast::ExprAST>> Parser::parseArgList() {
	auto expr = parseExpression();
	if (expr) {
		auto arg_list_t = parseArgListTail(std::move(expr));
		return arg_list_t;
	}
	return {};
}

std::unique_ptr<ast::ExprAST>
Parser::parsePostfixExprTail(std::unique_ptr<ast::ExprAST> primary_expr) {
	if (current.type == TokenType::LBRACKET) {
		advance();
		auto expr = parseExpression();
		if (expr) {
			if (current.type == TokenType::RBRACKET) {
				return expr;
			}
			errors.error("Expected ']' but got " + current.lexeme,
				     current.line, current.column);
			return nullptr;
		}
		return nullptr;
	}
	if (current.type == TokenType::LPAREN) {
		advance();

		auto args = parseArgList();
		if (current.type == TokenType::RPAREN) {
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
	case TokenType::STAR:
	case TokenType::SLASH:
	case TokenType::PERCENT:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::LESS_LESS:
	case TokenType::GREATER_GREATER:
	case TokenType::LESS:
	case TokenType::GREATER:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER_EQUAL:
	case TokenType::EQUAL_EQUAL:
	case TokenType::EXCLAMATION_EQUAL:
	case TokenType::AMPERSAND:
	case TokenType::CARET:
	case TokenType::PIPE:
	case TokenType::AMPERSAND_AMPERSAND:
	case TokenType::PIPE_PIPE:
	case TokenType::QUESTION:
	case TokenType::COMMA:
	case TokenType::RBRACKET:
	case TokenType::RPAREN:
	case TokenType::SEMICOLON:
		return primary_expr;
	default:
		errors.error(
		    "Expected '[', '(', '.', '::', '++', or '--' but got " +
			current.lexeme,
		    current.line, current.column);
		return nullptr;
	}
}

std::unique_ptr<ast::ExprAST> Parser::parsePostfixExpr() {
	std::unique_ptr<ast::ExprAST> lhs_opt = parsePrimaryExpr();
	if (!lhs_opt) {
		return nullptr;
	}

	std::unique_ptr<ast::ExprAST> lhs = std::move(lhs_opt);
	return parsePostfixExprTail(std::move(lhs));
}

std::unique_ptr<ast::ExprAST> Parser::parseUnaryExpr() {
	if (auto is_unary_op = unaryOperator(); is_unary_op) {
		ast::UnaryOp unary_op{};

		switch (current.type) {
		case TokenType::AMPERSAND:
			unary_op = ast::UnaryOp::AND;
			break;
		case TokenType::STAR:
			unary_op = ast::UnaryOp::MUL;
			break;
		case TokenType::PLUS:
			unary_op = ast::UnaryOp::PLUS;
			break;
		case TokenType::MINUS:
			unary_op = ast::UnaryOp::MINUS;
			break;
		case TokenType::EXCLAMATION:
			unary_op = ast::UnaryOp::NOT;
			break;
		case TokenType::TILDE:
			unary_op = ast::UnaryOp::BIT_NOT;
			break;
		default:
			errors.error("Invalid unary operator", current.line,
				     current.column);
			return nullptr;
		}
		advance();

		if (auto postfix_expr = parsePostfixExpr()) {
			return std::make_unique<ast::UnaryExprAST>(
			    unary_op, std::move(postfix_expr));
		}
		return nullptr;
	}
	return parsePostfixExpr();
}

static ast::BinaryOp tokenToBinaryOp(TokenType token) {
	switch (token) {
	case TokenType::PLUS:
		return ast::BinaryOp::ADD;
	case TokenType::MINUS:
		return ast::BinaryOp::SUB;
	case TokenType::STAR:
		return ast::BinaryOp::MUL;
	case TokenType::SLASH:
		return ast::BinaryOp::DIV;
	case TokenType::PERCENT:
		return ast::BinaryOp::MOD;
	case TokenType::EQUAL_EQUAL:
		return ast::BinaryOp::EQ;
	case TokenType::EXCLAMATION_EQUAL:
		return ast::BinaryOp::NEQ;
	case TokenType::LESS:
		return ast::BinaryOp::LT;
	case TokenType::GREATER:
		return ast::BinaryOp::GT;
	case TokenType::LESS_EQUAL:
		return ast::BinaryOp::LE;
	case TokenType::GREATER_EQUAL:
		return ast::BinaryOp::GE;
	case TokenType::AMPERSAND_AMPERSAND:
		return ast::BinaryOp::AND;
	case TokenType::PIPE_PIPE:
		return ast::BinaryOp::OR;
	case TokenType::AMPERSAND:
		return ast::BinaryOp::BIT_AND;
	case TokenType::PIPE:
		return ast::BinaryOp::BIT_OR;
	case TokenType::CARET:
		return ast::BinaryOp::BIT_XOR;
	case TokenType::LESS_LESS:
		return ast::BinaryOp::SHL;
	case TokenType::GREATER_GREATER:
		return ast::BinaryOp::SHR;
	default:
		assert(false &&
		       "tokenToBinaryOp: token is not a binary operator");
		std::unreachable();
	}
}

std::unique_ptr<ast::ExprAST> Parser::parseMultiplicativeExpr() {
	auto lhs = parseUnaryExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::STAR ||
	       current.type == TokenType::SLASH ||
	       current.type == TokenType::PERCENT) {

		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseUnaryExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseAdditiveExpr() {
	auto lhs = parseMultiplicativeExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::PLUS ||
	       current.type == TokenType::MINUS) {

		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseMultiplicativeExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseShiftExpr() {
	auto lhs = parseAdditiveExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::LESS_LESS ||
	       current.type == TokenType::GREATER_GREATER) {

		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseAdditiveExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseRelationalExpr() {
	auto lhs = parseShiftExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::LESS ||
	       current.type == TokenType::GREATER ||
	       current.type == TokenType::LESS_EQUAL ||
	       current.type == TokenType::GREATER_EQUAL) {

		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseShiftExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseEqualityExpr() {
	auto lhs = parseRelationalExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::EQUAL_EQUAL ||
	       current.type == TokenType::EXCLAMATION_EQUAL) {

		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseRelationalExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseAndExpr() {
	auto lhs = parseEqualityExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::AMPERSAND) {
		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseEqualityExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseXORExpr() {
	auto lhs = parseAndExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::CARET) {
		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseAndExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseInclusiveOrExpr() {
	auto lhs = parseXORExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::PIPE) {
		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseXORExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseLogicalAndExpr() {
	auto lhs = parseInclusiveOrExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::AMPERSAND_AMPERSAND) {
		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseInclusiveOrExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseLogicalOrExpr() {
	auto lhs = parseLogicalAndExpr();
	if (!lhs) {
		return nullptr;
	}

	while (current.type == TokenType::PIPE_PIPE) {
		ast::BinaryOp op = tokenToBinaryOp(current.type);
		advance();

		auto rhs = parseLogicalAndExpr();
		if (!rhs) {
			return nullptr;
		}

		lhs = std::make_unique<ast::BinaryExprAST>(op, std::move(lhs),
							   std::move(rhs));
	}
	return lhs;
}

std::unique_ptr<ast::ExprAST> Parser::parseExpression() {
	auto lhs = parseLogicalOrExpr();
	if (!lhs) {
		return nullptr;
	}
	return parseExpressionTail(std::move(lhs));
}

std::unique_ptr<ast::ExprAST>
Parser::parseExpressionTail(std::unique_ptr<ast::ExprAST> lhs) {
	if (current.type == TokenType::QUESTION) {
		advance();

		auto then_branch = parseExpression();
		if (then_branch != nullptr) {
			if (current.type == TokenType::COLON) {
				advance();
				auto else_branch = parseExpression();
				if (else_branch != nullptr) {
					return std::make_unique<
					    ast::TernaryExprAST>(
					    std::move(lhs),
					    std::move(then_branch),
					    std::move(else_branch));
				}
				return nullptr;
			}
			errors.error("Expected ':' but got: " + current.lexeme,
				     current.line, current.column);
			return nullptr;
		}
		return nullptr;
	}
	return lhs;
}

std::unique_ptr<ast::DeclAST> Parser::parseVarDecl() {
	if (current.type != TokenType::VAR) {
		errors.error("Expected 'var' got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	auto type = parseType();
	if (type == nullptr) {
		return nullptr;
	}
	if (current.type != TokenType::IDENT) {
		errors.error("Expected identifier but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	std::string name = current.lexeme;
	advance();

	// variable declaration tail
	if (current.type == TokenType::SEMICOLON) {
		advance();
		return std::make_unique<ast::VariableDeclarationAST>(
		    std::move(type), std::move(name));
	}
	if (current.type == TokenType::EQUAL) {
		advance();
		auto expr = parseExpression();
		if (current.type != TokenType::SEMICOLON) {
			errors.error("Expected ';' but got: " + current.lexeme,
				     current.line, current.column);
			advance();
			return nullptr;
		}
		advance();
		return std::make_unique<ast::VariableDeclarationAST>(
		    std::move(type), std::move(name), nullptr, std::move(expr));
	}
	// array variable
	if (current.type != TokenType::LBRACKET) {
		errors.error("Expected '[' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	auto array_size = parseExpression();
	if (current.type != TokenType::RBRACKET) {
		errors.error("Expected ']' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	if (current.type != TokenType::SEMICOLON) {
		errors.error("Expected ';' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::VariableDeclarationAST>(
	    std::move(type), std::move(name), std::move(array_size));
}

std::unique_ptr<ast::StmtAST> Parser::parseContinueStmt() {
	assert(current.type == TokenType::CONTINUE);
	advance();

	if (current.type != TokenType::SEMICOLON) {
		errors.error("Expected ';' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::ContinueStmtAST>();
}

std::unique_ptr<ast::StmtAST> Parser::parseBreakStmt() {
	assert(current.type == TokenType::BREAK);
	advance();

	if (current.type != TokenType::SEMICOLON) {
		errors.error("Expected ';' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::BreakStmtAST>();
}

std::unique_ptr<ast::StmtAST> Parser::parseReturnStmt() {
	assert(current.type == TokenType::RETURN);
	advance();

	if (current.type == TokenType::SEMICOLON) {
		advance();
		return std::make_unique<ast::ReturnStmtAST>();
	}

	auto ret_value = parseExpression();
	if (current.type != TokenType::SEMICOLON) {
		errors.error("Expected ';' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::ReturnStmtAST>(std::move(ret_value));
}

std::unique_ptr<ast::StmtAST> Parser::parseAssignmentStmt() {
	assert(current.type == TokenType::IDENT);
	std::string var_name = std::move(current.lexeme);
	advance();

	if (auto is_assignmement_op = assignmentOperator();
	    is_assignmement_op) {
		ast::AssignOp assignment_op{};

		switch (current.type) {
		case TokenType::EQUAL:
			assignment_op = ast::AssignOp::ASSIGN;
			break;
		case TokenType::PLUS_EQUAL:
			assignment_op = ast::AssignOp::ADD_ASSIGN;
			break;
		case TokenType::MINUS_EQUAL:
			assignment_op = ast::AssignOp::SUB_ASSIGN;
			break;
		case TokenType::STAR_EQUAL:
			assignment_op = ast::AssignOp::MUL_ASSIGN;
			break;
		case TokenType::SLASH_EQUAL:
			assignment_op = ast::AssignOp::DIV_ASSIGN;
			break;
		case TokenType::PERCENT_EQUAL:
			assignment_op = ast::AssignOp::MOD_ASSIGN;
			break;
		case TokenType::LESS_LESS_EQUAL:
			assignment_op = ast::AssignOp::SHL_ASSIGN;
			break;
		case TokenType::GREATER_GREATER_EQUAL:
			assignment_op = ast::AssignOp::SHR_ASSIGN;
			break;
		case TokenType::AMPERSAND_EQUAL:
			assignment_op = ast::AssignOp::BIT_AND_ASSIGN;
			break;
		case TokenType::CARET_EQUAL:
			assignment_op = ast::AssignOp::BIT_XOR_ASSIGN;
			break;
		case TokenType::PIPE_EQUAL:
			assignment_op = ast::AssignOp::BIT_OR_ASSIGN;
			break;
		default:
			errors.error("Invalid assignment operator: " +
					 current.lexeme,
				     current.line, current.column);
			advance();
			return nullptr;
		}
		advance();

		auto expr = parseExpression();

		if (current.type != TokenType::SEMICOLON) {
			errors.error("Expected ';' but got: " + current.lexeme,
				     current.line, current.column);
			advance();
			return nullptr;
		}
		advance();
		return std::make_unique<ast::AssignmentStmtAST>(
		    std::move(var_name), assignment_op, std::move(expr));
	}
	errors.error("Expected an assignment operator but got: " +
			 current.lexeme,
		     current.line, current.column);
	return nullptr;
}
std::unique_ptr<ast::StmtAST> Parser::parseWhileStmt() {
	assert(current.type == TokenType::WHILE);
	advance();

	if (current.type != TokenType::LPAREN) {
		errors.error("Expected '(' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto condition = parseExpression();
	if (condition == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::RPAREN) {
		errors.error("Expected ')' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto stmt_list = parseStmtList();
	if (stmt_list == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::WhileStmtAST>(std::move(condition),
						   std::move(stmt_list));
}

std::unique_ptr<ast::StmtAST> Parser::parseForInit() {
	switch (current.type) {
	case TokenType::VAR: {
		auto var_dec = parseVarDecl();
		if (var_dec == nullptr) {
			return nullptr;
		}
		return std::make_unique<ast::DeclStmtAST>(std::move(var_dec));
	}
	case TokenType::IDENT: {
		auto assignment = parseAssignmentStmt();
		if (assignment == nullptr) {
			return nullptr;
		}
		return assignment;
	}
	default: {
		errors.error("Expected variable declaration or assignment in "
			     "for-loop initializer but got: " +
				 current.lexeme,
			     current.line, current.column);
		return nullptr;
	}
	}
}

std::unique_ptr<ast::ExprAST> Parser::parseForUpdate() {
	auto expr = parseExpression();
	if (expr == nullptr) {
		return nullptr;
	}
	return expr;
}

std::unique_ptr<ast::StmtAST> Parser::parseForStmt() {
	assert(current.type == TokenType::FOR);
	advance();

	if (current.type != TokenType::LPAREN) {
		errors.error("Expected '(' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto for_init = parseForInit();
	if (for_init == nullptr) {
		return nullptr;
	}

	auto expr = parseExpression();
	if (expr == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::SEMICOLON) {
		errors.error("Expected ';' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto for_update = parseForUpdate();
	if (for_update == nullptr) {
		return nullptr;
	}
	if (current.type != TokenType::RPAREN) {
		errors.error("Expected ')' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto stmt_list = parseStmtList();
	if (stmt_list == nullptr) {
		return nullptr;
	}
	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	return std::make_unique<ast::ForStmtAST>(
	    std::move(for_init), std::move(expr), std::move(for_update),
	    std::move(stmt_list));
}

std::unique_ptr<ast::BlockStmtAST> Parser::parseElseStmt() {
	if (current.type == TokenType::IF) {
		auto if_stmt = parseIfStmt();
		if (if_stmt == nullptr) {
			return nullptr;
		}
		std::vector<std::unique_ptr<ast::StmtAST>> stmts;
		stmts.push_back(std::move(if_stmt));
		return std::make_unique<ast::BlockStmtAST>(std::move(stmts));
	}
	if (current.type != TokenType::LBRACE) {
		errors.error("Expected 'if' or '{' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto stmt_list = parseStmtList();
	if (stmt_list == nullptr) {
		return nullptr;
	}
	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return stmt_list;
}

std::unique_ptr<ast::BlockStmtAST> Parser::parseIfStmtTail() {
	if (current.type != TokenType::ELSE) {
		return nullptr;
	}
	advance();
	return parseElseStmt();
}

std::unique_ptr<ast::StmtAST> Parser::parseIfStmt() {
	assert(current.type == TokenType::IF);
	advance();

	if (current.type != TokenType::LPAREN) {
		errors.error("Expected '(' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto condition = parseExpression();
	if (condition == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::RPAREN) {
		errors.error("Expected ')' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto then_branch = parseStmtList();
	if (then_branch == nullptr) {
		return nullptr;
	}
	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme,
			     current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	auto else_branch = parseIfStmtTail();

	return std::make_unique<ast::IfStmtAST>(std::move(condition),
						std::move(then_branch),
						std::move(else_branch));
}

std::unique_ptr<ast::StmtAST> Parser::parseStmt() {
	switch (current.type) {
	case TokenType::IF: {
		return parseIfStmt();
	}
	case TokenType::FOR: {
		return parseForStmt();
	}
	case TokenType::WHILE: {
		return parseWhileStmt();
	}
	case TokenType::IDENT: {
		return parseAssignmentStmt();
	}
	case TokenType::RETURN: {
		return parseReturnStmt();
	}
	case TokenType::BREAK: {
		return parseBreakStmt();
	}
	case TokenType::CONTINUE: {
		return parseContinueStmt();
	}
	case TokenType::VAR: {
		auto variable = parseVarDecl();
		if (variable == nullptr) {
			return nullptr;
		}
		return std::make_unique<ast::DeclStmtAST>(std::move(variable));
	}
	default: {
		errors.error("Expected statement but got: " + current.lexeme,
			     current.line, current.column);
		return nullptr;
	}
	}
}

std::unique_ptr<ast::BlockStmtAST> Parser::parseStmtList() {
	std::vector<std::unique_ptr<ast::StmtAST>> stmts;

	while (current.type != TokenType::RBRACE &&
	       current.type != TokenType::T_EOF) {
		auto stmt = parseStmt();
		if (stmt == nullptr) {
			return nullptr;
		}
		stmts.push_back(std::move(stmt));
	}
	return std::make_unique<ast::BlockStmtAST>(std::move(stmts));
}

std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> Parser::parseParamList() {
	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params;
	auto param_type = parseType();
	if (param_type == nullptr) {
		return params;
	}
	if (current.type != TokenType::IDENT) {
		errors.error("Expected parameter name but got: " + current.lexeme, current.line, current.column);
		advance();
		return params;
	}
	params.emplace_back(std::move(param_type), std::move(current.lexeme));
	advance();

	// <param-list-tail>
	while (current.type == TokenType::COMMA) {
		advance();
		auto param_type = parseType();
		if (param_type == nullptr) {
			return params;
		}
		if (current.type != TokenType::IDENT) {
			errors.error("Expected parameter name but got: " + current.lexeme, current.line, current.column);
			advance();
			return params;
		}
		params.emplace_back(std::move(param_type), std::move(current.lexeme));
		advance();
	}
	return params;
}

std::unique_ptr<ast::PrototypeAST> Parser::parseProto() {
	assert(current.type == TokenType::FUNC);
	advance();

	if (current.type != TokenType::IDENT) {
		errors.error("Expected function identifier but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	auto func_name = parseQualifiedName();
	if (!func_name) {
		advance();
		return nullptr;
	}

	if (current.type != TokenType::LPAREN) {
		errors.error("Expected '(' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params;
	if (current.type != TokenType::RPAREN) {
		params = parseParamList();
	}
	if (current.type != TokenType::RPAREN) {
		errors.error("Expected ')' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	if (current.type != TokenType::ARROW) {
		errors.error("Expected '->' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto return_type = parseType();
	if (return_type == nullptr) {
		return nullptr;
	}
	return std::make_unique<ast::PrototypeAST>(std::move(*func_name), std::move(params), std::move(return_type));
}

std::unique_ptr<ast::DeclAST> Parser::parseFunc() {
	assert(current.type == TokenType::FUNC); // no advance

	auto prototype = parseProto();
	if ( prototype == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto body = parseStmtList();
	if (body == nullptr) {
		return nullptr;
	}

	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::FunctionAST>(std::move(prototype), std::move(body));
}


std::unique_ptr<ast::DeclAST> Parser::parseStruct() {
	assert(current.type == TokenType::STRUCT);
	advance();

	if (current.type != TokenType::IDENT) {
		errors.error("Expected struct name but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	std::string name = std::move(current.lexeme);
	advance();

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	// <struct-body>
	std::vector<std::unique_ptr<ast::VariableDeclarationAST>> fields;
	std::vector<std::unique_ptr<ast::FunctionAST>> methods;

	while (current.type == TokenType::VAR || current.type == TokenType::FUNC) {
		if (current.type == TokenType::VAR) {
			auto field = parseVarDecl();
			if (field == nullptr) {
				return nullptr;
			}
			fields.push_back(std::unique_ptr<ast::VariableDeclarationAST>(
			    static_cast<ast::VariableDeclarationAST *>(field.release())));
		} else {
			auto method = parseFunc();
			if (method == nullptr) {
				return nullptr;
			}
			methods.push_back(std::unique_ptr<ast::FunctionAST>(
			    static_cast<ast::FunctionAST *>(method.release())));
		}
	}

	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	return std::make_unique<ast::StructAST>(std::move(name), std::move(fields), std::move(methods));
}

std::unique_ptr<ast::DeclAST> Parser::parseNamespace() {
	assert(current.type == TokenType::NAMESPACE);
	advance();

	if (current.type != TokenType::IDENT) {
		errors.error("Expected namespace identifier but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	auto name = std::move(current.lexeme);
	advance();

	if (current.type != TokenType::LBRACE) {
		errors.error("Expected '{' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();

	auto decl_list = parseDeclList();
	if (!decl_list) {
		return nullptr;
	}

	if (current.type != TokenType::RBRACE) {
		errors.error("Expected '}' but got: " + current.lexeme, current.line, current.column);
		advance();
		return nullptr;
	}
	advance();
	return std::make_unique<ast::NamespaceAST>(std::move(name),
						   std::move(*decl_list));
}

std::unique_ptr<ast::DeclAST> Parser::parseDecl() {
	switch (current.type) {
	case TokenType::NAMESPACE:
		return parseNamespace();
	case TokenType::STRUCT:
		return parseStruct();
	case TokenType::FUNC:
		return parseFunc();
	default:
		errors.error(
		    "Expected 'namespace', 'struct', or 'func' but got: " +
			current.lexeme,
		    current.line, current.column);
		advance();
		return nullptr;
	}
}

std::optional<std::vector<std::unique_ptr<ast::DeclAST>>>
Parser::parseDeclList() {
	std::vector<std::unique_ptr<ast::DeclAST>> decls;

	while (current.type != TokenType::T_EOF && current.type != TokenType::RBRACE) {
		auto decl = parseDecl();
		if (decl == nullptr) {
			return std::nullopt;
		}
		decls.push_back(std::move(decl));
	}
	return decls;
}

std::unique_ptr<ast::ProgramAST> Parser::parseProgram() {
	auto decls = parseDeclList();
	if (!decls) {
		return nullptr;
	}
	if (current.type != TokenType::T_EOF) {
		errors.error("Expected declaration but got: " + current.lexeme,
			     current.line, current.column);
		return nullptr;
	}
	return std::make_unique<ast::ProgramAST>(std::move(*decls));
}

} // namespace frontend
