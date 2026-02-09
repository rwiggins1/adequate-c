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

// Declarations
class DeclAST : public ASTNode {};

}

