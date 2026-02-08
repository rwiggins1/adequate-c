#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include "frontend/ast/ast.hpp"

using namespace frontend;

TEST(astTest,  NumberExpr) {
	auto expr = std::make_unique<NumberLiteralAST>(42.0);
	ASSERT_NE(expr, nullptr);
	ASSERT_EQ(expr->getValue(), 42.0);
}

TEST(astTest, StringLiteral) {
	auto str_lit = std::make_unique<StringLiteralAST>("Hello, World!");
	ASSERT_NE(str_lit, nullptr);
	ASSERT_EQ(str_lit->getValue(), "Hello, World!");
}

TEST(astTest, CharLiteral) {
	auto char_lit = std::make_unique<CharLiteralAST>('a');
	ASSERT_NE(char_lit, nullptr);
	ASSERT_EQ(char_lit->getValue(), 'a');
}

TEST(astTest, BoolLiteral) {
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	ASSERT_NE(bool_lit, nullptr);
	ASSERT_EQ(bool_lit->getValue(), true);
}

TEST(astTest, UnaryExpr) {
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	auto unary = std::make_unique<UnaryExprAST>("+", std::move(bool_lit));
	ASSERT_EQ(unary->getOperator(), "+");
	ASSERT_NE(unary->getOperand(), nullptr);
}

// 2 + 3
TEST(astTest, BinaryExpr) {
	auto two = std::make_unique<NumberLiteralAST>(2);
	auto three = std::make_unique<NumberLiteralAST>(2);
	auto add = std::make_unique<BinaryExprAST>("+", std::move(two), std::move(three));
	ASSERT_EQ(add->getOperator(), "+");
	ASSERT_NE(add->getLhs(), nullptr);
	ASSERT_NE(add->getRhs(), nullptr);
}

TEST(astTest, VarDec) {
	auto expr = std::make_unique<NumberLiteralAST>(42.0);
	auto var_dec = std::make_unique<VariableDeclarationAST>("int", "x", std::move(expr));
	ASSERT_NE(var_dec, nullptr);
	ASSERT_EQ(var_dec->getType(), "int");
	ASSERT_EQ(var_dec->getName(), "x");
	ASSERT_NE(var_dec->getInit(), nullptr);
	ASSERT_EQ(var_dec->hasInit(), true);
}

