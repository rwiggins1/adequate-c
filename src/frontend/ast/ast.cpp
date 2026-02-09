#include <memory>
#include <string>
#include "ast.hpp"

namespace frontend::ast {

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init = nullptr)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
