#pragma once

#include "ast/ast.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
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

	std::unique_ptr<ast::ExprAST> parseExpression();
	std::unique_ptr<ast::ExprAST> parseNumberLiteral();
	std::unique_ptr<ast::ExprAST> parseStringLiteral();
	std::unique_ptr<ast::ExprAST> parseCharacterLiteral();
	std::unique_ptr<ast::ExprAST> parseBooleanLiteral();

	std::unique_ptr<ast::ExprAST> parseUnaryExpr();
	std::unique_ptr<ast::ExprAST> parseBinaryExpr();
	std::unique_ptr<ast::ExprAST> parseVarExpr();

	std::unique_ptr<ast::ExprAST> parseIdentifierExpr();
	std::unique_ptr<ast::ExprAST> parseFunctionCallExpr(std::string& name);

	std::unique_ptr<ast::ExprAST> parsePrimaryExpr();

	std::unique_ptr<ast::StmtAST> parseVariableDeclaration();

public:
	explicit Parser(Lexer &lex);
	std::unique_ptr<ast::StmtAST> parseVarDecl();
};
} // namespace frontend
