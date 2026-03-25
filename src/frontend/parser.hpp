#pragma once

#include "ast/ast.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <memory>
#include <optional>

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
	
	std::optional<std::unique_ptr<ast::ASTNode>> literal();
	[[nodiscard]] bool unaryOperator() const;
	[[nodiscard]] bool assignmentOperator() const;

	std::optional<std::unique_ptr<types::Type>> primitiveType();
	std::optional<std::unique_ptr<types::Type>> type();

	std::optional<std::unique_ptr<ast::ASTNode>> parseArgumentList();
	std::optional<std::unique_ptr<ast::ASTNode>> parseArgumentListTail();

	std::optional<std::unique_ptr<ast::ExprAST>> parseExpression();

	std::optional<std::unique_ptr<ast::ExprAST>> parseUnaryExpr();
	std::optional<std::unique_ptr<ast::ExprAST>> parseBinaryExpr();
	std::optional<std::unique_ptr<ast::ExprAST>> parseVarExpr();

	std::optional<std::unique_ptr<ast::ExprAST>> parseIdentifierExpr();
	std::optional<std::unique_ptr<ast::ExprAST>> parseFunctionCallExpr(std::string& name);

	std::optional<std::unique_ptr<ast::ASTNode>> primaryExpr();


public:
	explicit Parser(Lexer &lex, ErrorReporter &errors);
	std::optional<std::unique_ptr<ast::StmtAST>> parseVarDecl();
};
} // namespace frontend
