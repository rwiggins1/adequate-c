#pragma once

namespace frontend::ast {

// Expressions
class NumberLiteralAST;
class StringLiteralAST;
class CharLiteralAST;
class BoolLiteralAST;
class UnaryExprAST;
class BinaryExprAST;
class TernaryExprAST;
class VariableExprAST;
class CallExprAST;

// Statements
class BlockStmtAST;
class ReturnStmtAST;
class BreakStmtAST;
class ContinueStmtAST;
class AssignmentStmtAST;
class IfStmtAST;
class ForStmtAST;
class WhileStmtAST;
class DoStmtAST;

// Declarations
class DeclStmtAST;
class VariableDeclarationAST;
class FunctionAST;
class StructAST;
class NamespaceAST;
class ProgramAST;

class ASTVisitor {
public:
	ASTVisitor() = default;
	ASTVisitor(const ASTVisitor &) = default;
	ASTVisitor &operator=(const ASTVisitor &) = default;
	ASTVisitor(ASTVisitor &&) = default;
	ASTVisitor &operator=(ASTVisitor &&) = default;
	virtual ~ASTVisitor() = default;

	// Expressions
	virtual void visit(NumberLiteralAST &node) = 0;
	virtual void visit(StringLiteralAST &node) = 0;
	virtual void visit(CharLiteralAST &node) = 0;
	virtual void visit(BoolLiteralAST &node) = 0;
	virtual void visit(UnaryExprAST &node) = 0;
	virtual void visit(BinaryExprAST &node) = 0;
	virtual void visit(TernaryExprAST &node) = 0;
	virtual void visit(VariableExprAST &node) = 0;
	virtual void visit(CallExprAST &node) = 0;

	// Statements
	virtual void visit(BlockStmtAST &node) = 0;
	virtual void visit(ReturnStmtAST &node) = 0;
	virtual void visit(BreakStmtAST &node) = 0;
	virtual void visit(ContinueStmtAST &node) = 0;
	virtual void visit(AssignmentStmtAST &node) = 0;
	virtual void visit(IfStmtAST &node) = 0;
	virtual void visit(ForStmtAST &node) = 0;
	virtual void visit(WhileStmtAST &node) = 0;
	virtual void visit(DoStmtAST &node) = 0;

	// Declarations
	virtual void visit(DeclStmtAST &node) = 0;
	virtual void visit(VariableDeclarationAST &node) = 0;
	virtual void visit(FunctionAST &node) = 0;
	virtual void visit(StructAST &node) = 0;
	virtual void visit(NamespaceAST &node) = 0;
	virtual void visit(ProgramAST &node) = 0;
};

} // namespace frontend::ast
