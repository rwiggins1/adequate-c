#pragma once

#include "ast.hpp"

namespace frontend::ast {

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
		std::unique_ptr<ExprAST> initializer = nullptr);

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

}
