#pragma once

#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "types/type.hpp"
#include <memory>
#include <optional>

namespace frontend {
class Parser {
public:
	explicit Parser(Lexer &lex, ErrorReporter &errors);

	std::unique_ptr<ast::ExprAST> parseLiteral();
	[[nodiscard]] bool unaryOperator() const;
	[[nodiscard]] bool assignmentOperator() const;

	std::unique_ptr<types::Type> parsePrimitiveType();
	std::unique_ptr<types::Type> parseType();

	std::vector<std::unique_ptr<ast::ExprAST>> parseArgList();
	std::vector<std::unique_ptr<ast::ExprAST>>
	    parseArgListTail(std::unique_ptr<ast::ExprAST>);

	std::unique_ptr<ast::ExprAST> parseUnaryExpr();
	std::unique_ptr<ast::ExprAST> parseVarExpr();
	std::unique_ptr<ast::ExprAST> parsePostfixExpr();
	std::unique_ptr<ast::ExprAST>
	    parsePostfixExprTail(std::unique_ptr<ast::ExprAST>);

	std::unique_ptr<ast::ExprAST> parsePrimaryExpr();

	std::unique_ptr<ast::ExprAST> parseMultiplicativeExpr();
	std::unique_ptr<ast::ExprAST> parseAdditiveExpr();
	std::unique_ptr<ast::ExprAST> parseShiftExpr();
	std::unique_ptr<ast::ExprAST> parseRelationalExpr();
	std::unique_ptr<ast::ExprAST> parseEqualityExpr();
	std::unique_ptr<ast::ExprAST> parseAndExpr();
	std::unique_ptr<ast::ExprAST> parseXORExpr();
	std::unique_ptr<ast::ExprAST> parseInclusiveOrExpr();
	std::unique_ptr<ast::ExprAST> parseLogicalAndExpr();
	std::unique_ptr<ast::ExprAST> parseLogicalOrExpr();

	std::unique_ptr<ast::ExprAST> parseExpression();
	std::unique_ptr<ast::ExprAST>
	    parseExpressionTail(std::unique_ptr<ast::ExprAST>);

	std::unique_ptr<ast::DeclAST> parseVarDecl();
	std::unique_ptr<ast::StmtAST> parseContinueStmt();
	std::unique_ptr<ast::StmtAST> parseBreakStmt();
	std::unique_ptr<ast::StmtAST> parseReturnStmt();
	std::unique_ptr<ast::StmtAST> parseAssignmentStmt();
	std::unique_ptr<ast::StmtAST> parseWhileStmt();

	std::unique_ptr<ast::StmtAST> parseForStmt();
	std::unique_ptr<ast::StmtAST> parseForInit();
	std::unique_ptr<ast::ExprAST> parseForUpdate();

	std::unique_ptr<ast::StmtAST> parseIfStmt();
	std::unique_ptr<ast::BlockStmtAST> parseIfStmtTail();
	std::unique_ptr<ast::BlockStmtAST> parseElseStmt();

	std::unique_ptr<ast::StmtAST> parseStmt();
	std::unique_ptr<ast::BlockStmtAST> parseStmtList();


	std::unique_ptr<ast::DeclAST> parseFunc();
	std::unique_ptr<ast::DeclAST> parseStruct();
	std::unique_ptr<ast::DeclAST> parseNamespace();
	std::unique_ptr<ast::DeclAST> parseDecl();
	std::unique_ptr<ast::ProgramAST> parseDeclList();

	std::unique_ptr<ast::ProgramAST> parseProgram();

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
