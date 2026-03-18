#pragma once

#include "../types/type.hpp"
#include "ast.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace frontend::ast {
// Variable Declaration
class VariableDeclarationAST : public DeclAST {
	std::unique_ptr<types::Type> type;
	std::string name;
	std::unique_ptr<ExprAST> initializer;

public:
	VariableDeclarationAST(std::unique_ptr<types::Type> type,
			       std::string name,
			       std::unique_ptr<ExprAST> initializer = nullptr);

	[[nodiscard]] types::Type *getType() const noexcept {
		return type.get();
	}
	[[nodiscard]] std::string getName() const noexcept { return name; }
	[[nodiscard]] ExprAST *getInit() const { return initializer.get(); }
	[[nodiscard]] bool hasInit() const noexcept {
		return initializer != nullptr;
	}
};

/// PrototypeAST - Function signature
/// Captures: name, parameters with types, and return type
class PrototypeAST {
	std::string name;
	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
	    params;
	std::unique_ptr<types::Type> returnType;

public:
	PrototypeAST(
	    std::string name,
	    std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
		params,
	    std::unique_ptr<types::Type> returnType);

	[[nodiscard]] const std::string &getName() const { return name; }
	[[nodiscard]] const auto &getParams() const { return params; }
	[[nodiscard]] const types::Type *getReturnType() const {
		return returnType.get();
	}
};

class FunctionAST : public DeclAST {
	std::unique_ptr<PrototypeAST> prototype;
	std::vector<std::unique_ptr<StmtAST>> body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
		    std::vector<std::unique_ptr<StmtAST>> body);

	[[nodiscard]] const PrototypeAST *getProto() const {
		return prototype.get();
	}
	[[nodiscard]] const std::vector<std::unique_ptr<StmtAST>> &
	getBody() const {
		return body;
	}
};

class StructAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;
	std::vector<std::unique_ptr<FunctionAST>> methods;

public:
	StructAST(std::string name,
		  std::vector<std::unique_ptr<VariableDeclarationAST>> fields =
		      std::vector<std::unique_ptr<VariableDeclarationAST>>{},

		  std::vector<std::unique_ptr<FunctionAST>> methods =
		      std::vector<std::unique_ptr<FunctionAST>>{});

	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] const std::vector<
	    std::unique_ptr<VariableDeclarationAST>> &
	getFields() const {
		return fields;
	}
	[[nodiscard]] const std::vector<std::unique_ptr<FunctionAST>> &
	getMethods() const {
		return methods;
	}
};

class NamespaceAST : public DeclAST {
	std::string name;
	std::vector<std::unique_ptr<DeclAST>> declarations;

public:
	NamespaceAST(std::string name,
		     std::vector<std::unique_ptr<DeclAST>> declarations);
	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] const std::vector<std::unique_ptr<DeclAST>> &
	getDeclarations() const {
		return declarations;
	}
};

} // namespace frontend::ast
