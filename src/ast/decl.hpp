#pragma once

#include "../types/type.hpp"
#include "ast.hpp"
#include "visitor.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace frontend::ast {
class DeclStmtAST : public StmtAST {
	std::unique_ptr<DeclAST> decl;

public:
	explicit DeclStmtAST(std::unique_ptr<DeclAST> decl);
	[[nodiscard]] DeclAST *getDecl() const { return decl.get(); }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

// Variable Declaration
class VariableDeclarationAST : public DeclAST {
	std::unique_ptr<types::Type> type;
	std::string name;
	std::unique_ptr<ExprAST> size; // array[length + 1]
	std::unique_ptr<ExprAST> initializer;

public:
	VariableDeclarationAST(std::unique_ptr<types::Type> type,
			       std::string name,
			       std::unique_ptr<ExprAST> size = nullptr,
			       std::unique_ptr<ExprAST> initializer = nullptr);

	[[nodiscard]] types::Type *getType() const noexcept {
		return type.get();
	}
	[[nodiscard]] std::string getName() const noexcept { return name; }
	[[nodiscard]] ExprAST *getInit() const { return initializer.get(); }
	[[nodiscard]] bool hasInit() const noexcept {
		return initializer != nullptr;
	}
	[[nodiscard]] ExprAST *getArraySize() const noexcept {
		return size.get();
	}
	[[nodiscard]] bool isArray() const noexcept { return size != nullptr; }
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

/// PrototypeAST - Function signature
/// Captures: name, parameters with types, and return type
class PrototypeAST {
	QualifiedName name;
	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
	    params;
	std::unique_ptr<types::Type> returnType;

public:
	PrototypeAST(
	    QualifiedName name,
	    std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
		params,
	    std::unique_ptr<types::Type> returnType);
	PrototypeAST(
	    std::string name,
	    std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
		params,
	    std::unique_ptr<types::Type> returnType);

	[[nodiscard]] const std::string &getName() const { return name.name; }
	[[nodiscard]] const QualifiedName &getQualifiedName() const noexcept {
		return name;
	}
	[[nodiscard]] const auto &getParams() const { return params; }
	[[nodiscard]] const types::Type *getReturnType() const {
		return returnType.get();
	}
};

class FunctionAST : public DeclAST {
	std::unique_ptr<PrototypeAST> prototype;
	std::unique_ptr<BlockStmtAST> body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
		    std::unique_ptr<BlockStmtAST> body);
	// Defined in decl.cpp where BlockStmtAST is complete, so the
	// unique_ptr<BlockStmtAST> destructor isn't instantiated here
	~FunctionAST() override;
	FunctionAST(FunctionAST &&) noexcept;
	FunctionAST &operator=(FunctionAST &&) noexcept;
	FunctionAST(const FunctionAST &) = delete;
	FunctionAST &operator=(const FunctionAST &) = delete;

	[[nodiscard]] const PrototypeAST *getProto() const {
		return prototype.get();
	}
	[[nodiscard]] const std::unique_ptr<BlockStmtAST> &getBody() const {
		return body;
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
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
	void accept(ASTVisitor &v) override { v.visit(*this); }
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
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

class ProgramAST : public DeclAST {
	std::vector<std::unique_ptr<DeclAST>> declarations;

public:
	explicit ProgramAST(
	    std::vector<std::unique_ptr<DeclAST>> declarations = {});
	[[nodiscard]] const std::vector<std::unique_ptr<DeclAST>> &
	getDeclarations() const {
		return declarations;
	}
	void accept(ASTVisitor &v) override { v.visit(*this); }
};

} // namespace frontend::ast
