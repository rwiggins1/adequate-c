#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace frontend {
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

class NumberExprAST: public ExprAST {
	double value;
public:
	NumberExprAST(double val);
	[[nodiscard]] double getValue() const noexcept { return value; }
};

class StringLiteralAST: public ExprAST {
	std::string value;
public:
	StringLiteralAST(std::string value);
	[[nodiscard]] std::string getValue() const { return value; };
};

class CharLiteralAST: public ExprAST {
	char value;
public:
	CharLiteralAST(const char& value);
	[[nodiscard]] char getValue() const noexcept { return value; }
};

class BoolLiteralAST : public ExprAST {
	bool value;
public:
	BoolLiteralAST(bool value);
	[[nodiscard]] bool getValue() const noexcept { return value; }
};

class UnaryExprAST: public ExprAST {
	char op;
	std::unique_ptr<ExprAST> operand;
public:
	UnaryExprAST(char op, std::unique_ptr<ExprAST> operand);
};

class BinaryExprAST: public ExprAST {
	char op;
	std::unique_ptr<ExprAST> lhs, rhs;
public:
	BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs,
	       std::unique_ptr<ExprAST> rhs);
};

class VariableExprAST: public ExprAST {
	std::string name;
public:
	VariableExprAST(const std::string& name);
};

class CallExprAST: public ExprAST {
	std::string callee;
	std::vector<std::unique_ptr<ExprAST>> args;
public:
	CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args);
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

class AsignmentStmtAST: public StmtAST {
	std::string varName;
	std::unique_ptr<ExprAST> value;
public:
	AsignmentStmtAST(std::string varName, std::unique_ptr<ExprAST>value);
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

class SwitchStmtAST : public StmtAST{
	std::unique_ptr<ExprAST> value;
	std::vector<std::pair<std::unique_ptr<ExprAST>, std::unique_ptr<BlockStmtAST>>> cases;
	std::unique_ptr<BlockStmtAST> defaultCase;

	SwitchStmtAST(std::unique_ptr<ExprAST> value,
	       std::vector<std::pair<std::unique_ptr<ExprAST>, 
	       std::unique_ptr<BlockStmtAST>>> cases,
	       std::unique_ptr<BlockStmtAST> defaultCase = nullptr);
};

class StructAST: public ASTNode {
	std::string name;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;

	StructAST(std::string name,
		std::vector<std::unique_ptr<VariableDeclarationAST>> fields);
};

class EnumAST : public ASTNode{
	std::string name;
	std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> elements;

	EnumAST(std::string name,
		std::vector<std::pair<std::string,
	 	std::unique_ptr<ExprAST>>> elements);
};

class ClassAST : public ASTNode{
	std::string name;
	std::string parent;
	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;
	std::vector<std::unique_ptr<FunctionAST>> methods;

	ClassAST(std::string name, std::string parent, 
		std::vector<std::unique_ptr<VariableDeclarationAST>> fields,
		std::vector<std::unique_ptr<FunctionAST>> methods);
};
}

