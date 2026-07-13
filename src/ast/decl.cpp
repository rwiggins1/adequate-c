#include "ast/decl.hpp"
#include "ast/ast.hpp"
#include "stmt.hpp"
#include <memory>
#include <string>

namespace frontend::ast {
DeclStmtAST::DeclStmtAST(std::unique_ptr<DeclAST> decl)
    : decl(std::move(decl)) {}

VariableDeclarationAST::VariableDeclarationAST(
    std::unique_ptr<types::Type> type, std::string name,
    std::unique_ptr<ExprAST> size, std::unique_ptr<ExprAST> init)
    : type(std::move(type)), name(std::move(name)), size(std::move(size)),
      initializer(std::move(init)) {}

PrototypeAST::PrototypeAST(
    QualifiedName name,
    std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params,
    std::unique_ptr<types::Type> returnType)
    : name(std::move(name)), params(std::move(params)),
      returnType(std::move(returnType)) {}

PrototypeAST::PrototypeAST(
    std::string name,
    std::vector<std::pair<std::unique_ptr<types::Type>, std::string>> params,
    std::unique_ptr<types::Type> returnType)
    : name(std::move(name)), params(std::move(params)),
      returnType(std::move(returnType)) {}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> prototype,
			 std::unique_ptr<BlockStmtAST> body)
    : prototype(std::move(prototype)), body(std::move(body)) {}

FunctionAST::~FunctionAST() = default;
FunctionAST::FunctionAST(FunctionAST &&) noexcept = default;
FunctionAST &FunctionAST::operator=(FunctionAST &&) noexcept = default;

StructAST::StructAST(
    std::string name,
    std::vector<std::unique_ptr<VariableDeclarationAST>> fields,
    std::vector<std::unique_ptr<FunctionAST>> methods)
    : name(std::move(name)), fields(std::move(fields)),
      methods(std::move(methods)) {}

NamespaceAST::NamespaceAST(std::string name,
			   std::vector<std::unique_ptr<DeclAST>> declarations)
    : name(std::move(name)), declarations(std::move(declarations)) {}

ProgramAST::ProgramAST(std::vector<std::unique_ptr<DeclAST>> declarations)
    : declarations(std::move(declarations)) {}
} // namespace frontend::ast
