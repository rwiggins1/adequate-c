#include "printer.hpp"
#include "decl.hpp"
#include "expr.hpp"
#include "stmt.hpp"

#include <string>

namespace frontend::ast {

namespace {

const char *toString(UnaryOp op) {
	switch (op) {
	case UnaryOp::AND:
		return "&";
	case UnaryOp::MUL:
		return "*";
	case UnaryOp::PLUS:
		return "+";
	case UnaryOp::MINUS:
		return "-";
	case UnaryOp::NOT:
		return "!";
	case UnaryOp::BIT_NOT:
		return "~";
	case UnaryOp::POST_INCREMENT:
		return "++ (post)";
	case UnaryOp::POST_DECREMENT:
		return "-- (post)";
	}
	return "?";
}

const char *toString(BinaryOp op) {
	switch (op) {
	case BinaryOp::ADD:
		return "+";
	case BinaryOp::SUB:
		return "-";
	case BinaryOp::MUL:
		return "*";
	case BinaryOp::DIV:
		return "/";
	case BinaryOp::MOD:
		return "%";
	case BinaryOp::EQ:
		return "==";
	case BinaryOp::NEQ:
		return "!=";
	case BinaryOp::LT:
		return "<";
	case BinaryOp::GT:
		return ">";
	case BinaryOp::LE:
		return "<=";
	case BinaryOp::GE:
		return ">=";
	case BinaryOp::AND:
		return "&&";
	case BinaryOp::OR:
		return "||";
	case BinaryOp::BIT_AND:
		return "&";
	case BinaryOp::BIT_OR:
		return "|";
	case BinaryOp::BIT_XOR:
		return "^";
	case BinaryOp::SHL:
		return "<<";
	case BinaryOp::SHR:
		return ">>";
	}
	return "?";
}

const char *toString(AssignOp op) {
	switch (op) {
	case AssignOp::ASSIGN:
		return "=";
	case AssignOp::ADD_ASSIGN:
		return "+=";
	case AssignOp::SUB_ASSIGN:
		return "-=";
	case AssignOp::MUL_ASSIGN:
		return "*=";
	case AssignOp::DIV_ASSIGN:
		return "/=";
	case AssignOp::MOD_ASSIGN:
		return "%=";
	case AssignOp::SHL_ASSIGN:
		return "<<=";
	case AssignOp::SHR_ASSIGN:
		return ">>=";
	case AssignOp::BIT_AND_ASSIGN:
		return "&=";
	case AssignOp::BIT_XOR_ASSIGN:
		return "^=";
	case AssignOp::BIT_OR_ASSIGN:
		return "|=";
	}
	return "?";
}

std::string signature(const PrototypeAST &proto) {
	std::string out = proto.getQualifiedName().str() + "(";
	bool first = true;
	for (const auto &[type, name] : proto.getParams()) {
		if (!first) {
			out += ", ";
		}
		first = false;
		out += type->toString() + " " + name;
	}
	out += ") -> ";
	out += proto.getReturnType()->toString();
	return out;
}

} // namespace

std::ostream &ASTPrinter::line() {
	for (int i = 0; i < depth; ++i) {
		out << "  ";
	}
	return out;
}

void ASTPrinter::child(ASTNode *node) {
	if (node == nullptr) {
		return;
	}
	++depth;
	node->accept(*this);
	--depth;
}

// Expressions

void ASTPrinter::visit(NumberLiteralAST &node) {
	line() << "NumberLiteral " << node.getValue() << "\n";
}

void ASTPrinter::visit(StringLiteralAST &node) {
	line() << "StringLiteral \"" << node.getValue() << "\"\n";
}

void ASTPrinter::visit(CharLiteralAST &node) {
	line() << "CharLiteral '" << node.getValue() << "'\n";
}

void ASTPrinter::visit(BoolLiteralAST &node) {
	line() << "BoolLiteral " << (node.getValue() ? "true" : "false")
	       << "\n";
}

void ASTPrinter::visit(UnaryExprAST &node) {
	line() << "UnaryExpr " << toString(node.getOperator()) << "\n";
	child(node.getOperand());
}

void ASTPrinter::visit(BinaryExprAST &node) {
	line() << "BinaryExpr " << toString(node.getOperator()) << "\n";
	child(node.getLhs());
	child(node.getRhs());
}

void ASTPrinter::visit(TernaryExprAST &node) {
	line() << "TernaryExpr\n";
	child(node.getCondition());
	child(node.getThenBranch());
	child(node.getElseBranch());
}

void ASTPrinter::visit(VariableExprAST &node) {
	line() << "VariableExpr " << node.getQualifiedName().str() << "\n";
}

void ASTPrinter::visit(CallExprAST &node) {
	line() << "CallExpr\n";
	child(node.getCallee());
	for (const auto &arg : node.getArgs()) {
		child(arg.get());
	}
}

// Statements

void ASTPrinter::visit(BlockStmtAST &node) {
	line() << "Block\n";
	for (const auto &stmt : node.getStmts()) {
		child(stmt.get());
	}
}

void ASTPrinter::visit(ReturnStmtAST &node) {
	line() << "ReturnStmt\n";
	child(node.getValue());
}

void ASTPrinter::visit(BreakStmtAST & /*node*/) { line() << "BreakStmt\n"; }

void ASTPrinter::visit(ContinueStmtAST & /*node*/) {
	line() << "ContinueStmt\n";
}

void ASTPrinter::visit(AssignmentStmtAST &node) {
	line() << "AssignmentStmt " << node.getVariableName() << " "
	       << toString(node.getOperator()) << "\n";
	child(node.getValue());
}

void ASTPrinter::visit(IfStmtAST &node) {
	line() << "IfStmt\n";
	child(node.getCondition());
	child(node.getThenBranch());
	if (node.hasElse()) {
		child(node.getElseBranch());
	}
}

void ASTPrinter::visit(ForStmtAST &node) {
	line() << "ForStmt\n";
	child(node.getInit());
	child(node.getCondition());
	child(node.getUpdate());
	child(node.getBody());
}

void ASTPrinter::visit(WhileStmtAST &node) {
	line() << "WhileStmt\n";
	child(node.getCondition());
	child(node.getBody());
}

void ASTPrinter::visit(DoStmtAST &node) {
	line() << "DoStmt\n";
	child(node.getBody());
	child(node.getCondition());
}

// Declarations

void ASTPrinter::visit(DeclStmtAST &node) {
	line() << "DeclStmt\n";
	child(node.getDecl());
}

void ASTPrinter::visit(VariableDeclarationAST &node) {
	line() << "VarDecl " << node.getType()->toString() << " "
	       << node.getName() << (node.isArray() ? "[]" : "") << "\n";
	child(node.getArraySize());
	child(node.getInit());
}

void ASTPrinter::visit(FunctionAST &node) {
	line() << "FunctionDecl " << signature(*node.getProto()) << "\n";
	child(node.getBody().get());
}

void ASTPrinter::visit(StructAST &node) {
	line() << "StructDecl " << node.getName() << "\n";
	for (const auto &field : node.getFields()) {
		child(field.get());
	}
	for (const auto &method : node.getMethods()) {
		child(method.get());
	}
}

void ASTPrinter::visit(NamespaceAST &node) {
	line() << "NamespaceDecl " << node.getName() << "\n";
	for (const auto &decl : node.getDeclarations()) {
		child(decl.get());
	}
}

void ASTPrinter::visit(ProgramAST &node) {
	line() << "Program\n";
	for (const auto &decl : node.getDeclarations()) {
		child(decl.get());
	}
}

} // namespace frontend::ast
