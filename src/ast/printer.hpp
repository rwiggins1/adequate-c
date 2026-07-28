#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <iostream>

namespace frontend::ast {

/// Dumps the AST as an indented tree, one node per line
/// (clang -ast-dump style). Usage: ASTPrinter(out).print(node);
class ASTPrinter final : public ASTVisitor {
public:
	explicit ASTPrinter(std::ostream &out = std::cout) : out(out) {}

	void print(ASTNode &node) { node.accept(*this); }

	// Expressions
	void visit(NumberLiteralAST &node) override;
	void visit(StringLiteralAST &node) override;
	void visit(CharLiteralAST &node) override;
	void visit(BoolLiteralAST &node) override;
	void visit(UnaryExprAST &node) override;
	void visit(BinaryExprAST &node) override;
	void visit(TernaryExprAST &node) override;
	void visit(VariableExprAST &node) override;
	void visit(CallExprAST &node) override;

	// Statements
	void visit(BlockStmtAST &node) override;
	void visit(ReturnStmtAST &node) override;
	void visit(BreakStmtAST &node) override;
	void visit(ContinueStmtAST &node) override;
	void visit(AssignmentStmtAST &node) override;
	void visit(IfStmtAST &node) override;
	void visit(ForStmtAST &node) override;
	void visit(WhileStmtAST &node) override;
	void visit(DoStmtAST &node) override;

	// Declarations
	void visit(DeclStmtAST &node) override;
	void visit(VariableDeclarationAST &node) override;
	void visit(FunctionAST &node) override;
	void visit(StructAST &node) override;
	void visit(NamespaceAST &node) override;
	void visit(ProgramAST &node) override;

private:
	std::ostream &out;
	int depth = 0;

	// Prints the indent for the current depth; the caller streams the
	// rest of the line into the returned ostream.
	std::ostream &line();
	// Visits a child one level deeper; safe to call with nullptr.
	void child(ASTNode *node);
};

} // namespace frontend::ast
