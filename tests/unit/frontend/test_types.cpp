
#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/type.hpp"
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

