#pragma once

#include "ast/ast.hpp"
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
	std::unique_ptr<ExprAST> parseNumberLiteral();
	std::unique_ptr<ExprAST> parseStringLiteral();
	std::unique_ptr<ExprAST> parseCharacterLiteral();
	std::unique_ptr<ExprAST> parseBooleanLiteral();

	std::unique_ptr<ExprAST> parseUnaryExpr();
	std::unique_ptr<ExprAST> parseBinaryExpr();
	std::unique_ptr<ExprAST> parseVarExpr();

	std::unique_ptr<ExprAST> parseIdentifierExpr();
	std::unique_ptr<ExprAST> parseFunctionCallExpr(std::string& name);

	std::unique_ptr<ExprAST> parsePrimaryExpr();

	std::unique_ptr<StmtAST> parseVariableDeclaration();

public:
	explicit Parser(Lexer &lex);
	std::unique_ptr<StmtAST> parseVarDecl();
};
} // namespace frontend
