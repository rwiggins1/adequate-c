#include <memory>
#include <string>
#include "stmt.hpp"

namespace frontend::ast {

VariableDeclarationAST::VariableDeclarationAST(std::unique_ptr<types::Type> type,
					       std::string name,
					       std::unique_ptr<ExprAST> init)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}


}
