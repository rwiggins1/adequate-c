#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace frontend::ast {
// Base Class
class ASTNode {
public:
	virtual ~ASTNode() = default;

	ASTNode(const ASTNode&) = delete;
	ASTNode& operator=(const ASTNode&) = delete;

	ASTNode(ASTNode&&) = default;
	ASTNode& operator=(ASTNode&&) = default;
protected:
	ASTNode() = default;
};

// Expressions
class ExprAST: public ASTNode {
protected:
	ExprAST() = default;
};

// Statements
class StmtAST : public ASTNode {};

class BlockStmtAST : public StmtAST {
	std::vector<std::unique_ptr<StmtAST>> statements;
public:
	BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts);
};

class ReturnStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> value;
public:
	ReturnStmtAST(std::unique_ptr<ExprAST> value);
};

class BreakStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> value;
public:
	BreakStmtAST(std::unique_ptr<ExprAST> value);
};

class AssignmentStmtAST: public StmtAST {
	std::string varName;
	std::unique_ptr<ExprAST> value;
public:
	AssignmentStmtAST(std::string varName, std::unique_ptr<ExprAST>value);
};

// Variable Declaration
class VariableDeclarationAST : public StmtAST {
	std::string type;
	std::string name;
	std::unique_ptr<ExprAST> initializer;
public:
	VariableDeclarationAST(std::string type, std::string name, 
		std::unique_ptr<ExprAST> initializer);

	[[nodiscard]] std::string getType() const noexcept { return type; }
	[[nodiscard]] std::string getName() const noexcept { return name; }
	[[nodiscard]] ExprAST* getInit() const { return initializer.get(); }
	[[nodiscard]] bool hasInit() const noexcept { return initializer != nullptr; }
};

class IfStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<BlockStmtAST> thenBranch;
	std::unique_ptr<BlockStmtAST> elseBranch;
public:
	IfStmtAST(std::unique_ptr<ExprAST> condition, 
	       std::unique_ptr<BlockStmtAST> thenBranch,
	       std::unique_ptr<BlockStmtAST> elseBranch = nullptr);
};

class ForStmtAST : public StmtAST {
	std::unique_ptr<VariableDeclarationAST> initialization;
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<ExprAST> update;
	std::unique_ptr<BlockStmtAST> body;
public:
	ForStmtAST(std::unique_ptr<VariableDeclarationAST> initialization,
		std::unique_ptr<ExprAST> condition, std::unique_ptr<ExprAST> update,
	        std::unique_ptr<BlockStmtAST> body);
};

class WhileStmtAST : public StmtAST {
	std::unique_ptr<ExprAST> condition;
	std::unique_ptr<BlockStmtAST> body;
public:
	WhileStmtAST(std::unique_ptr<ExprAST> condition,
	      std::unique_ptr<BlockStmtAST> body);
};

class DoStmtAST : public StmtAST {
	std::unique_ptr<BlockStmtAST> body;
	std::unique_ptr<ExprAST> condition;
public:
	DoStmtAST(std::unique_ptr<BlockStmtAST> body,
	       std::unique_ptr<ExprAST> condition);
};

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

class FunctionAST {
	std::unique_ptr<PrototypeAST> prototype;
	std::unique_ptr<BlockStmtAST> body;
public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
	     std::unique_ptr<BlockStmtAST> body);

	[[nodiscard]] const PrototypeAST* getProto() const { return prototype.get(); }
};

class StructAST: public ASTNode {
	std::string name;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;

	StructAST(std::string name,
		std::vector<std::unique_ptr<VariableDeclarationAST>> fields);
};
}

