#include <gtest/gtest.h>
#include <memory>
#include "frontend/ast.hpp"

using namespace frontend;

TEST(astTest,  NumberExpr) {
	auto expr = std::make_unique<NumberExprAST>(42.0);
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

TEST(astTest, VarDec) {
	auto expr = std::make_unique<NumberExprAST>(42.0);
	auto var_dec = std::make_unique<VariableDeclarationAST>("int", "x", std::move(expr));
}
