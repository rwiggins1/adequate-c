#include "parser.hpp"
#include "frontend/token.hpp"
#include <iostream>
#include <memory>

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

std::unique_ptr<ExprAST> Parser::parseNumberLiteral() {
	double value = std::stod(current.lexeme);
	advance();
	return std::make_unique<NumberLiteralAST>(value);
}

std::unique_ptr<ExprAST> Parser::parseStringLiteral() {
	std::string value = current.lexeme;
	advance();
	return std::make_unique<StringLiteralAST>(value);
}

std::unique_ptr<ExprAST> Parser::parseCharacterLiteral() {
	std::string str = current.lexeme;
	const char *value = str.c_str();
	advance();
	return std::make_unique<CharLiteralAST>(*value);
}

std::unique_ptr<ExprAST> Parser::parseBooleanLiteral() {
	bool value = current.type == TokenType::TRUE;
	advance();
	return std::make_unique<BoolLiteralAST>(value);
}

std::unique_ptr<VariableDeclarationAST> Parser::parseVarDecl() {
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

	std::unique_ptr<ExprAST> initializer = nullptr;

	if (match(TokenType::ASSIGN)) {
		advance(); // consume '='

		if (match(TokenType::NUMBER)) {
			initializer = parseNumberLiteral();
		}
		else if (match(TokenType::STRING_LIT)) {
			initializer = parseStringLiteral();
		}
		else if (match(TokenType::CHAR_LIT)) {
			initializer = parseCharacterLiteral();
		}
		else if (match(TokenType::TRUE) || match(TokenType::FALSE)) {
			initializer = parseBooleanLiteral();
		}
		else {
			std::cerr << "Expected expression after '='\n";
			return nullptr;
		}
	}

	if (!expect(TokenType::SEMICOLON)) {
		return nullptr;
	}
	advance(); // consume ';'

	return std::make_unique<VariableDeclarationAST>(type, name,
							std::move(initializer));
}

} // namespace frontend
