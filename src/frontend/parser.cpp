#include "parser.hpp"
#include "lexer/token.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace frontend {

Parser::Parser(Lexer &lex) : lexer(lex), current(lexer.get()) {}

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

std::unique_ptr<ast::ExprAST> Parser::parseNumberLiteral() {
	double value = std::stod(current.lexeme);
	advance();
	return std::make_unique<ast::NumberLiteralAST>(value);
}

std::unique_ptr<ast::ExprAST> Parser::parseStringLiteral() {
	std::string value = current.lexeme;
	advance();
	return std::make_unique<ast::StringLiteralAST>(value);
}

std::unique_ptr<ast::ExprAST> Parser::parseCharacterLiteral() {
	std::string str = current.lexeme;
	const char *value = str.c_str();
	advance();
	return std::make_unique<ast::CharLiteralAST>(*value);
}

std::unique_ptr<ast::ExprAST> Parser::parseBooleanLiteral() {
	bool value = current.type == TokenType::TRUE;
	advance();
	return std::make_unique<ast::BoolLiteralAST>(value);
}

std::unique_ptr<ast::ExprAST> Parser::parseUnaryExpr() {
	std::string op;
	std::unique_ptr<ast::ExprAST> operand = nullptr;
	
	// if post-fix operator (identifier++)
	if (current.type == TokenType::IDENT) {
		operand = parsePrimaryExpr();
		advance();
		op = current.lexeme;
		return std::make_unique<ast::UnaryExprAST>(op, std::move(operand));
	}

	// if pre-fixed operator (!identifier)
	op = current.lexeme;
	advance();
	operand = parsePrimaryExpr();
	return std::make_unique<ast::UnaryExprAST>(op, std::move(operand));
}

// TODO: Add nested Expr support (Operator precedence)
// TODO: Use Maximal Munch
std::unique_ptr<ast::ExprAST> Parser::parseBinaryExpr() {
	std::unique_ptr<ast::ExprAST> lhs = parsePrimaryExpr();
	std::string op = current.lexeme;
	advance();
	std::unique_ptr<ast::ExprAST> rhs = parsePrimaryExpr();
	return std::make_unique<ast::BinaryExprAST>(op, std::move(lhs), std::move(rhs));
}

std::unique_ptr<ast::ExprAST> Parser::parseVarExpr() {
	std::string name = current.lexeme;
	advance();
	return std::make_unique<ast::VariableExprAST>(name);
}

std::unique_ptr<ast::ExprAST> Parser::parseIdentifierExpr() {
	std::string name = current.lexeme;
	advance(); // eats identifier

	if (match(TokenType::OPAREN)) {
		return parseFunctionCallExpr(name);
	}

	return std::make_unique<ast::VariableExprAST>(name);
}

// TODO: Allow function calls as arguments
std::unique_ptr<ast::ExprAST> Parser::parseFunctionCallExpr(std::string& name) {
	advance(); // eat (
	std::vector<std::unique_ptr<ast::ExprAST>> args; 
	
	while (!match(TokenType::CPAREN)) {
		if (auto arg = parsePrimaryExpr()) {
			args.push_back(std::move(arg));
		}
		else {
			return nullptr;
		}

		if (match(TokenType::COMMA)) {
			advance();
			if (match(TokenType::CPAREN)) { // trailing comma: foo(x,)
				return nullptr; 
			}
		}
		else if (!match(TokenType::CPAREN)) { // if not ',' must be ')'
			return nullptr;
		}
	}
	advance();
	return std::make_unique<ast::CallExprAST>(name, std::move(args));
}

std::unique_ptr<ast::ExprAST> Parser::parsePrimaryExpr() {
	switch (current.type) {
		case TokenType::NUMBER:
			return parseNumberLiteral();
		case TokenType::STRING_LIT:
			return parseStringLiteral();
		case TokenType::CHAR_LIT:
			return parseCharacterLiteral();

		case TokenType::TRUE:
		case TokenType::FALSE:
			return parseBooleanLiteral();

		case TokenType::IDENT:
			return parseIdentifierExpr();

		case TokenType::OPAREN:
			return nullptr; // TODO: add parseParenExpr()
		default:
			return nullptr;
	}
}

std::unique_ptr<ast::StmtAST> Parser::parseVarDecl() {
	// int test = 3;

	if (!isType(current.type)) {
		return nullptr;
	}
	std::string type = current.lexeme;
	advance();

	if (!expect(TokenType::IDENT)) {
		return nullptr;
	}
	std::string name = current.lexeme;
	advance();

	std::unique_ptr<ast::ExprAST> initializer = nullptr;

	if (match(TokenType::ASSIGN)) {
		advance(); // consume '='
		initializer = parsePrimaryExpr();
	}

	if (!expect(TokenType::SEMICOLON)) {
		return nullptr;
	}
	advance(); // consume ';'

	return std::make_unique<ast::VariableDeclarationAST>(type, name,
							std::move(initializer));
}

} // namespace frontend
