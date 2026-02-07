#include <algorithm>
#include <gtest/gtest.h>
#include "frontend/lexer.hpp"
#include "frontend/token.hpp"

using namespace::frontend;

TEST(lexerTest, Get) {
	Lexer lexer("int variable = 2 + 3");
	Token token = lexer.get();
	ASSERT_EQ(token.type, TokenType::INT);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::IDENT);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::ASSIGN);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::NUMBER);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::PLUS);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::NUMBER);
}

