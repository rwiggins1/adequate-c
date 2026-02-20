#pragma once

#include "ast.hpp"
#include "stmt.hpp"
#include "../types/type.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace frontend::ast {
/// PrototypeAST - Function signature
/// Captures: name, parameters with types, and return type
class PrototypeAST {
	std::string name;
	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params;
	std::unique_ptr<types::Type> returnType;
public:
	PrototypeAST(std::string name,
	      std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params,
	      std::unique_ptr<types::Type> returnType);

	[[nodiscard]] const std::string& getName() const { return name; }
	[[nodiscard]] const auto& getParams() const { return params; }
	[[nodiscard]] const types::Type* getReturnType() const { return returnType.get(); }
};

class FunctionAST : public DeclAST {
	std::unique_ptr<PrototypeAST> prototype;
	std::vector<std::unique_ptr<StmtAST>> body;
public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
	     std::vector<std::unique_ptr<StmtAST>> body);

	[[nodiscard]] const PrototypeAST* getProto() const { return prototype.get(); }
	[[nodiscard]] const std::vector<std::unique_ptr<StmtAST>>& getBody() const { return body; }
};

class StructAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;
public:
	StructAST(std::string name,
		std::vector<std::unique_ptr<VariableDeclarationAST>> fields);

	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] const std::vector<std::unique_ptr<VariableDeclarationAST>>& getDeclarations() const 
	{ return fields; }
};

class NamespaceAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<DeclAST>> declarations;
public:
	NamespaceAST(std::string name, std::vector<std::unique_ptr<DeclAST>> declarations);
	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] const std::vector<std::unique_ptr<DeclAST>>& getDeclarations() const { return declarations; }
};

}
