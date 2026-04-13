#pragma once

#include "ast/ast.hpp"
#include "diagnostics/diagnostics.hpp"
#include "ast/expr.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <memory>
#include <optional>

namespace frontend {
class Parser {
public:
	explicit Parser(Lexer &lex, ErrorReporter &errors);

	std::optional<std::unique_ptr<ast::ExprAST>> parseLiteral();
	[[nodiscard]] bool unaryOperator() const;
	[[nodiscard]] bool assignmentOperator() const;

	std::optional<std::unique_ptr<types::Type>> parsePrimitiveType();
	std::optional<std::unique_ptr<types::Type>> parseType();

	std::vector<std::unique_ptr<ast::ExprAST>> parseArgList();
	std::vector<std::unique_ptr<ast::ExprAST>>
	    parseArgListTail(std::unique_ptr<ast::ExprAST>);

	std::optional<std::unique_ptr<ast::ExprAST>> parseExpression();

	std::optional<std::unique_ptr<ast::ExprAST>> parseMultiplicativeExpr();

	std::optional<std::unique_ptr<ast::ExprAST>> parseUnaryExpr();
	std::optional<std::unique_ptr<ast::ExprAST>> parseVarExpr();
	std::optional<std::unique_ptr<ast::ExprAST>> parsePostfixExpr();
	std::optional<std::unique_ptr<ast::ExprAST>>
	    parsePostfixExprTail(std::unique_ptr<ast::ExprAST>);

	std::optional<std::unique_ptr<ast::ExprAST>> parsePrimaryExpr();

	std::optional<std::unique_ptr<ast::StmtAST>> parseVarDecl();

private:
	Lexer &lexer;
	Token current;
	ErrorReporter &errors;

	void advance();
	bool match(TokenType type);
	static bool isType(TokenType type);
	static bool isLiteral(TokenType type);
	bool expect(TokenType type);
};
} // namespace frontend
