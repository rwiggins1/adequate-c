#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace frontend::ast {

class BlockStmtAST : public StmtAST {
	std::vector<std::unique_ptr<StmtAST>> statements;

public:
	BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts = {});
	[[nodiscard]] const std::vector<std::unique_ptr<StmtAST>> &
	getStmts() const {
		return statements;
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class ReturnStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> value;

public:
	ReturnStmtAST(std::unique_ptr<ExprAST> value = nullptr);
	[[nodiscard]] ExprAST *getValue() const { return value.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class BreakStmtAST : public StmtAST {
public:
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class ContinueStmtAST : public StmtAST {
public:
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

enum class AssignOp : uint8_t {
	ASSIGN,		// =
	ADD_ASSIGN,	// +=
	SUB_ASSIGN,	// -=
	MUL_ASSIGN,	// *=
	DIV_ASSIGN,	// /=
	MOD_ASSIGN,	// %=
	SHL_ASSIGN,	// <<=
	SHR_ASSIGN,	// >>=
	BIT_AND_ASSIGN, // &=
	BIT_XOR_ASSIGN, // ^=
	BIT_OR_ASSIGN,	// |=
};

class AssignmentStmtAST : public StmtAST {
	std::string varName;
	AssignOp op;
	std::unique_ptr<ExprAST> value;

public:
	AssignmentStmtAST(std::string varName, AssignOp op,
			  std::unique_ptr<ExprAST> value);
	[[nodiscard]] std::string getVariableName() const { return varName; }
	[[nodiscard]] AssignOp getOperator() const noexcept { return op; }
	[[nodiscard]] ExprAST *getValue() const { return value.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class IfStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<BlockStmtAST> thenBranch;
	std::unique_ptr<BlockStmtAST> elseBranch;

public:
	IfStmtAST(std::unique_ptr<ExprAST> condition,
		  std::unique_ptr<BlockStmtAST> thenBranch,
		  std::unique_ptr<BlockStmtAST> elseBranch = nullptr);
	[[nodiscard]] ExprAST *getCondition() const { return condition.get(); }
	[[nodiscard]] BlockStmtAST *getThenBranch() const {
		return thenBranch.get();
	}
	[[nodiscard]] BlockStmtAST *getElseBranch() const {
		return elseBranch.get();
	}
	[[nodiscard]] bool hasElse() const noexcept {
		return elseBranch != nullptr;
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class ForStmtAST : public StmtAST {
	std::unique_ptr<StmtAST> initialization;
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<ExprAST> update;
	std::unique_ptr<BlockStmtAST> body;

public:
	ForStmtAST(std::unique_ptr<StmtAST> initialization,
		   std::unique_ptr<ExprAST> condition,
		   std::unique_ptr<ExprAST> update,
		   std::unique_ptr<BlockStmtAST> body);
	[[nodiscard]] StmtAST *getInit() const { return initialization.get(); }
	[[nodiscard]] bool hasInit() const noexcept {
		return initialization != nullptr;
	}
	[[nodiscard]] ExprAST *getCondition() const { return condition.get(); }
	[[nodiscard]] ExprAST *getUpdate() const { return update.get(); }
	[[nodiscard]] bool hasUpdate() const noexcept {
		return update != nullptr;
	}
	[[nodiscard]] BlockStmtAST *getBody() const { return body.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class WhileStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<BlockStmtAST> body;

public:
	WhileStmtAST(std::unique_ptr<ExprAST> condition,
		     std::unique_ptr<BlockStmtAST> body);
	[[nodiscard]] ExprAST *getCondition() const { return condition.get(); }
	[[nodiscard]] BlockStmtAST *getBody() const { return body.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class DoStmtAST : public StmtAST {
	std::unique_ptr<BlockStmtAST> body;
	std::unique_ptr<ExprAST> condition;

public:
	DoStmtAST(std::unique_ptr<BlockStmtAST> body,
		  std::unique_ptr<ExprAST> condition);
	[[nodiscard]] BlockStmtAST *getBody() const { return body.get(); }
	[[nodiscard]] ExprAST *getCondition() const { return condition.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

} // namespace frontend::ast
