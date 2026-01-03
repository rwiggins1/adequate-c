#include "ast.hpp"
#include <algorithm>
#include <memory>
#include <string>

namespace frontend {
NumberLiteralAST::NumberLiteralAST(double val) : value(val) {}

StringLiteralAST::StringLiteralAST(std::string val) : value(std::move(val)) {}

CharLiteralAST::CharLiteralAST(const char& val) : value(val) {}

BoolLiteralAST::BoolLiteralAST(bool val): value(val) {}

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init = nullptr)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
