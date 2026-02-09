#pragma once

#include "ast.hpp"
#include "stmt.hpp"
#include <memory>
#include <string>
#include <vector>

namespace frontend::ast {
/// PrototypeAST - Function signature
/// Captures: name, parameters with types, and return type
class PrototypeAST {
	std::string name;
	std::vector<std::pair<std::string, std::string>> params;
	std::string returnType;
public:
	PrototypeAST(std::string name,
	      std::vector<std::pair<std::string, std::string>> params,
	      std::string returnType);

	[[nodiscard]] const std::string& getName() const { return name; }
	[[nodiscard]] const auto& getParams() const { return params; }
	[[nodiscard]] const std::string& getReturnType() const { return returnType; }
};

class FunctionAST : public DeclAST {
	std::unique_ptr<PrototypeAST> prototype;
	std::vector<std::unique_ptr<StmtAST>> body;
public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
	     std::vector<std::unique_ptr<StmtAST>> body);

	[[nodiscard]] const PrototypeAST* getProto() const { return prototype.get(); }
};

class StructAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;

	StructAST(std::string name,
		std::vector<std::unique_ptr<VariableDeclarationAST>> fields);
	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] std::vector<std::unique_ptr<VariableDeclarationAST>> getDeclarations() const 
	{ return fields; }
};

class NamespaceAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<DeclAST>> declarations;
public:
	NamespaceAST(std::string name, std::vector<std::unique_ptr<DeclAST>> declarations);
	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] std::vector<std::unique_ptr<DeclAST>> getDeclarations() const { return declarations; }
};

}
