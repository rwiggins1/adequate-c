#include "expr.hpp"
#include <utility>

namespace frontend::ast {

NumberLiteralAST::NumberLiteralAST(double val) : value(val) {}

StringLiteralAST::StringLiteralAST(std::string val) : value(std::move(val)) {}

CharLiteralAST::CharLiteralAST(const char& val) : value(val) {}

BoolLiteralAST::BoolLiteralAST(bool val): value(val) {}

UnaryExprAST::UnaryExprAST(UnaryOp op, std::unique_ptr<ExprAST> operand)
	: op(op), operand(std::move(operand)) {}

BinaryExprAST::BinaryExprAST(BinaryOp op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
	: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

TernaryExprAST::TernaryExprAST(std::unique_ptr<ExprAST> condition, std::unique_ptr<ExprAST> thenBranch, std::unique_ptr<ExprAST> elseBranch)
    : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

VariableExprAST::VariableExprAST(std::string name) : name(std::move(name)) {}

CallExprAST::CallExprAST(std::unique_ptr<ExprAST> callee, std::vector<std::unique_ptr<ExprAST>> args)
	: callee(std::move(callee)), args(std::move(args)) {}
}
