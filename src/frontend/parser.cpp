#include "parser.hpp"
#include <iostream>

namespace frontend {

Parser::Parser(Lexer &lex) : lexer(lex), current(lexer.get()) {}

void Parser::advance() { current = lexer.get(); }

bool Parser::match(TokenType type) { return current.type == type; }

bool Parser::expect(TokenType type) {
	if (!match(type)) {
		std::cerr << "Expected " << static_cast<int>(type)
			  << " but got " << static_cast<int>(current.type)
			  << "\n";
		return false;
	}
	return true;
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
	double value = std::stod(current.lexeme);
	advance();
	return std::make_unique<NumberExprAST>(value);
}

std::unique_ptr<VariableDeclarationAST> Parser::parseVarDecl() {
	// int test = 3;

	if (!match(TokenType::INT)) {
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
			initializer = parseNumberExpr();
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
