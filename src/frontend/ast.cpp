#include "ast.hpp"
#include <algorithm>
#include <memory>

namespace frontend {
NumberExprAST::NumberExprAST(double val) : value(val) {}

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
