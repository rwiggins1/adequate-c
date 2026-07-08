#pragma once

#include <ast/ast.hpp>
#include <ast/visitor.hpp>
#include <cstdint>
#include <memory>

namespace frontend::ast {

class NumberLiteralAST : public ExprAST {
	double value;

public:
	NumberLiteralAST(double val);
	[[nodiscard]] double getValue() const noexcept { return value; }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class StringLiteralAST : public ExprAST {
	std::string value;

public:
	StringLiteralAST(std::string value);
	[[nodiscard]] std::string getValue() const { return value; };
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class CharLiteralAST : public ExprAST {
	char value;

public:
	CharLiteralAST(const char &value);
	[[nodiscard]] char getValue() const noexcept { return value; }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class BoolLiteralAST : public ExprAST {
	bool value;

public:
	BoolLiteralAST(bool value);
	[[nodiscard]] bool getValue() const noexcept { return value; }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

enum class UnaryOp : uint8_t {
	AND,
	MUL,
	PLUS,
	MINUS,
	NOT,
	BIT_NOT,
	POST_INCREMENT,
	POST_DECREMENT
};

class UnaryExprAST : public ExprAST {
	UnaryOp op;
	std::unique_ptr<ExprAST> operand;

public:
	UnaryExprAST(UnaryOp op, std::unique_ptr<ExprAST> operand);
	[[nodiscard]] UnaryOp getOperator() const noexcept { return op; }
	[[nodiscard]] ExprAST *getOperand() const { return operand.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

enum class BinaryOp : uint8_t {
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	EQ,
	NEQ,
	LT,
	GT,
	LE,
	GE,
	AND,
	OR,
	BIT_AND,
	BIT_OR,
	BIT_XOR,
	SHL,
	SHR
};

class BinaryExprAST : public ExprAST {
	BinaryOp op;
	std::unique_ptr<ExprAST> lhs, rhs;

public:
	BinaryExprAST(BinaryOp op, std::unique_ptr<ExprAST> lhs,
		      std::unique_ptr<ExprAST> rhs);

	[[nodiscard]] BinaryOp getOperator() const noexcept { return op; }
	[[nodiscard]] ExprAST *getLhs() const { return lhs.get(); }
	[[nodiscard]] ExprAST *getRhs() const { return rhs.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class TernaryExprAST : public ExprAST {
	std::unique_ptr<ExprAST> condition, thenBranch, elseBranch;

public:
	TernaryExprAST(std::unique_ptr<ExprAST> condition,
		       std::unique_ptr<ExprAST> thenBranch,
		       std::unique_ptr<ExprAST> elseBranch);

	[[nodiscard]] ExprAST *getCondition() const { return condition.get(); }
	[[nodiscard]] ExprAST *getThenBranch() const {
		return thenBranch.get();
	}
	[[nodiscard]] ExprAST *getElseBranch() const {
		return elseBranch.get();
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class VariableExprAST : public ExprAST {
	std::string name;

public:
	VariableExprAST(std::string name);
	[[nodiscard]] std::string getName() const noexcept { return name; }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class CallExprAST : public ExprAST {
	std::unique_ptr<ExprAST> callee;
	std::vector<std::unique_ptr<ExprAST>> args;

public:
	CallExprAST(std::unique_ptr<ExprAST> callee,
		    std::vector<std::unique_ptr<ExprAST>> args);
	[[nodiscard]] ExprAST *getCallee() const noexcept {
		return callee.get();
	}
	[[nodiscard]] const std::vector<std::unique_ptr<ExprAST>> &
	getArgs() const {
		return args;
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

} // namespace frontend::ast
