#include <gtest/gtest.h>
#include <memory>
#include "frontend/ast.hpp"

using namespace frontend;

TEST(astTest,  NumberExpr) {
	auto expr = std::make_unique<NumberExprAST>(42.0);
	ASSERT_NE(expr, nullptr);
	ASSERT_EQ(expr->getValue(), 42.0);
}
