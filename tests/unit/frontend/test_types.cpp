
#include "frontend/ast/ast.hpp"
#include "frontend/ast/decl.hpp"
#include "frontend/ast/expr.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/diagnostics/diagnostics.hpp"
#include "frontend/lexer/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/types/type.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
#include <locale>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>
#include <iostream>

using namespace frontend;
using namespace frontend::ast;
using namespace frontend::types;

TEST(Types, IntegerType) {
	auto int_type = std::make_unique<types::IntType>();
	ASSERT_NE(int_type, nullptr);

	ASSERT_EQ(int_type->toString(), "int");

	// test equals()
	auto type = std::make_unique<types::FloatType>();
	ASSERT_FALSE(int_type->equals(type.release()));

	// check if primitive type
	ASSERT_TRUE(int_type->isPrimitive());
}

