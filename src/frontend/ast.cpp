#include "ast.hpp"
#include <algorithm>
#include <memory>
#include <string>

namespace frontend {
NumberExprAST::NumberExprAST(double val) : value(val) {}

double NumberExprAST::getValue() {
	return value;
}

StringLiteralAST::StringLiteralAST(std::string val) : value(std::move(val)) {}

std::string StringLiteralAST::getValue() {
	return value;
}

CharLiteralAST::CharLiteralAST(const char& val) : value(val) {}

char CharLiteralAST::getValue() {
	return value;
}

BoolLiteralAST::BoolLiteralAST(bool val): value(val) {}

bool BoolLiteralAST::getValue() {
	return value;
}

VariableDeclarationAST::VariableDeclarationAST(std::string type,
					       std::string name,
					       std::unique_ptr<ExprAST> init)
	: type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
}
