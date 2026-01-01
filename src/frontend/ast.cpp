#include "ast.hpp"
#include <algorithm>
#include <memory>

namespace frontend {
NumberExprAST::NumberExprAST(double val) : value(val) {}

StringLiteralAST::StringLiteralAST(std::string val) : value(std::move(val)) {}

CharLiteralAST::CharLiteralAST(const char& val) : value(val) {}

BoolLiteralAST::BoolLiteralAST(bool val): value(val) {}

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
