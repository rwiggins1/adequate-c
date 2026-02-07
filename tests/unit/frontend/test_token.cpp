#include <gtest/gtest.h>
#include "frontend/lexer.hpp"
#include "frontend/token.hpp"

using namespace::frontend;

TEST(tokenTest, token) {
	Token token(TokenType::INT, "int", 2, 4);

	ASSERT_EQ(token.type, TokenType::INT);
	ASSERT_EQ(token.lexeme, "int");
	ASSERT_EQ(token.line, 2);
	ASSERT_EQ(token.column, 4);
}
