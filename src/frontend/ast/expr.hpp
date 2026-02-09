#pragma once

#include <frontend/ast/ast.hpp>

namespace frontend::ast {

class NumberLiteralAST: public ExprAST {
	double value;
public:
	NumberLiteralAST(double val);
	[[nodiscard]] double getValue() const noexcept { return value; }
};

class StringLiteralAST: public ExprAST {
	std::string value;
public:
	StringLiteralAST(std::string value);
	[[nodiscard]] std::string getValue() const { return value; };
};

class CharLiteralAST: public ExprAST {
	char value;
public:
	CharLiteralAST(const char& value);
	[[nodiscard]] char getValue() const noexcept { return value; }
};

class BoolLiteralAST : public ExprAST {
	bool value;
public:
	BoolLiteralAST(bool value);
	[[nodiscard]] bool getValue() const noexcept { return value; }
};

class UnaryExprAST: public ExprAST {
	std::string op;
	std::unique_ptr<ExprAST> operand;
public:
	UnaryExprAST(std::string op, std::unique_ptr<ExprAST> operand);
	[[nodiscard]] std::string getOperator() const noexcept { return op; }
	[[nodiscard]] ExprAST* getOperand() const { return operand.get(); }
};

class BinaryExprAST: public ExprAST {
	std::string op;
	std::unique_ptr<ExprAST> lhs, rhs;
public:
	BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs,
	       std::unique_ptr<ExprAST> rhs);
	
	[[nodiscard]] std::string getOperator() const noexcept { return op; }
	[[nodiscard]] ExprAST* getLhs() const { return lhs.get(); }
	[[nodiscard]] ExprAST* getRhs() const { return rhs.get(); }
};

class VariableExprAST: public ExprAST {
	std::string name;
public:
	VariableExprAST(std::string name);
	[[nodiscard]] std::string getName() const noexcept { return name; }
};

class CallExprAST: public ExprAST {
	std::string callee;
	std::vector<std::unique_ptr<ExprAST>> args;
public:
	CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args);
	[[nodiscard]] std::string getCallee() const noexcept { return callee; }
	[[nodiscard]] const std::vector<std::unique_ptr<ExprAST>>& getArgs() const { return args; }
};

}
