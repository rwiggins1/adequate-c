#include "gtest/gtest.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include "frontend/ast/ast.hpp"
#include "frontend/ast/expr.hpp"
#include "frontend/ast/stmt.hpp"

using namespace frontend::ast;

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
	auto unary = std::make_unique<UnaryExprAST>("!", std::move(bool_lit));
	ASSERT_EQ(unary->getOperator(), "!");

	ExprAST* literal = unary->getOperand();
	auto* boolLiteral = dynamic_cast<BoolLiteralAST*>(literal);

	ASSERT_EQ(boolLiteral->getValue(), true);
}

// 2 + 3
TEST(astTest, BinaryExpr) {
	auto two = std::make_unique<NumberLiteralAST>(2);
	auto three = std::make_unique<NumberLiteralAST>(3);
	auto add = std::make_unique<BinaryExprAST>("+", std::move(two), std::move(three));
	ASSERT_EQ(add->getOperator(), "+");

	ExprAST* lhs = add->getLhs();
	auto* lhsLiteral = dynamic_cast<NumberLiteralAST*>(lhs);
	ASSERT_EQ(lhsLiteral->getValue(), 2);

	ExprAST* rhs = add->getRhs();
	auto* rhsLiteral = dynamic_cast<NumberLiteralAST*>(rhs);

	ASSERT_EQ(rhsLiteral->getValue(), 3);
}

// someFunc(12, "test", true)
TEST(astTest, FunctionCallExpr) {
	std::string callee = "someFunc";
	auto twelve = std::make_unique<NumberLiteralAST>(12);
	auto testStr = std::make_unique<StringLiteralAST>("test");
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	std::vector<std::unique_ptr<ExprAST>> args;

	args.push_back(std::move(twelve));
	args.push_back(std::move(testStr));
	args.push_back(std::move(bool_lit));

	auto funcCall = std::make_unique<CallExprAST>(callee, std::move(args));
	const auto& Args = funcCall->getArgs();

	auto* TwelveLiteral = dynamic_cast<NumberLiteralAST*>(Args[0].get());

	ASSERT_EQ(TwelveLiteral->getValue(), 12);
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

