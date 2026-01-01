#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <memory>

namespace frontend {
class Parser {
private:
	Lexer &lexer;
	Token current;

	void advance();
	bool match(TokenType type);
	static bool isType(TokenType type);
	static bool isLiteral(TokenType type);
	bool expect(TokenType type);

	std::unique_ptr<ExprAST> parseExpression();
	std::unique_ptr<ExprAST> parseNumberExpr();
	std::unique_ptr<ExprAST> parseStringLiteral();
	std::unique_ptr<ExprAST> parseCharacterLiteral();
	std::unique_ptr<ExprAST> parseBooleanLiteral();
	std::unique_ptr<VariableDeclarationAST> parseVariableDeclaration();

public:
	explicit Parser(Lexer &lex);
	std::unique_ptr<VariableDeclarationAST> parseVarDecl();
};
} // namespace frontend
