#pragma once

#include "ast/ast.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <memory>

namespace frontend {
class Parser {
private:
	Lexer &lexer;
	Token current;
	ErrorReporter &errors;

	void advance();
	bool match(TokenType type);
	static bool isType(TokenType type);
	static bool isLiteral(TokenType type);
	bool expect(TokenType type);
	
	std::unique_ptr<ast::ASTNode> literal();
	[[nodiscard]] bool unaryOperator() const;
	[[nodiscard]] bool assignmentOperator() const;

	std::unique_ptr<types::Type> primitiveType();
	std::unique_ptr<types::Type> type();

	std::unique_ptr<ast::ASTNode> parseArgumentList();
	std::unique_ptr<ast::ASTNode> parseArgumentListTail();

	std::unique_ptr<ast::ExprAST> parseExpression();

	std::unique_ptr<ast::ExprAST> parseUnaryExpr();
	std::unique_ptr<ast::ExprAST> parseBinaryExpr();
	std::unique_ptr<ast::ExprAST> parseVarExpr();

	std::unique_ptr<ast::ExprAST> parseIdentifierExpr();
	std::unique_ptr<ast::ExprAST> parseFunctionCallExpr(std::string& name);

	std::unique_ptr<ast::ASTNode> primaryExpr();

	std::unique_ptr<ast::StmtAST> parseVariableDeclaration();

public:
	explicit Parser(Lexer &lex, ErrorReporter &errors);
	std::unique_ptr<ast::StmtAST> parseVarDecl();
};
} // namespace frontend
