#include "frontend/ast/decl.hpp"
#include <memory>
#include <string>
#include "stmt.hpp"

namespace frontend::ast {

VariableDeclarationAST::VariableDeclarationAST(std::unique_ptr<types::Type> type,
					       std::string name,
					       std::unique_ptr<ExprAST> init)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}

PrototypeAST::PrototypeAST(std::string name,
			   std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params,
			   std::unique_ptr<types::Type> returnType) 
	: name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)) {}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> prototype,
			 std::vector<std::unique_ptr<StmtAST>> body) 
	: prototype(std::move(prototype)), body(std::move(body)) {}

StructAST::StructAST(std::string name,
		     std::vector<std::unique_ptr<VariableDeclarationAST>> fields,
		     std::vector<std::unique_ptr<FunctionAST>> methods) 
	: name(std::move(name)), fields(std::move(fields)), methods(std::move(methods)) {}

NamespaceAST::NamespaceAST(std::string name,
			   std::vector<std::unique_ptr<DeclAST>> declarations) 
	: name(std::move(name)), declarations(std::move(declarations)) {}
}
