#include "stmt.hpp"
#include <memory>
#include <string>

namespace frontend::ast {

BlockStmtAST::BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts)
    : statements(std::move(stmts)) {}

ReturnStmtAST::ReturnStmtAST(std::unique_ptr<ExprAST> value)
    : value(std::move(value)) {}

BreakStmtAST::BreakStmtAST(std::unique_ptr<ExprAST> value)
    : value(std::move(value)) {}

AssignmentStmtAST::AssignmentStmtAST(std::string varName,
				     std::unique_ptr<ExprAST> value)
    : varName(std::move(varName)), value(std::move(value)) {}

IfStmtAST::IfStmtAST(std::unique_ptr<ExprAST> condition,
		     std::unique_ptr<BlockStmtAST> thenBranch,
		     std::unique_ptr<BlockStmtAST> elseBranch)
    : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

ForStmtAST::ForStmtAST(std::unique_ptr<VariableDeclarationAST> initialization,
		       std::unique_ptr<ExprAST> condition,
		       std::unique_ptr<ExprAST> update,
		       std::unique_ptr<BlockStmtAST> body)
    : initialization(std::move(initialization)),
      condition(std::move(condition)), update(std::move(update)),
      body(std::move(body)) {}

WhileStmtAST::WhileStmtAST(std::unique_ptr<ExprAST> condition,
			   std::unique_ptr<BlockStmtAST> body)
    : condition(std::move(condition)), body(std::move(body)) {}
} // namespace frontend::ast
