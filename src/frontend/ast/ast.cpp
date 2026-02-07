#include "ast.hpp"
#include <memory>
#include <string>
#include <vector>

namespace frontend {
NumberLiteralAST::NumberLiteralAST(double val) : value(val) {}

StringLiteralAST::StringLiteralAST(std::string val) : value(std::move(val)) {}

CharLiteralAST::CharLiteralAST(const char& val) : value(val) {}

BoolLiteralAST::BoolLiteralAST(bool val): value(val) {}

UnaryExprAST::UnaryExprAST(std::string op, std::unique_ptr<ExprAST> operand) 
	: op(std::move(op)), operand(std::move(operand)) {}

BinaryExprAST::BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) 
	: op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

VariableExprAST::VariableExprAST(std::string name) : name(std::move(name)) {}

CallExprAST::CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args) 
	: callee(std::move(callee)), args(std::move(args)) {}

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init = nullptr)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
