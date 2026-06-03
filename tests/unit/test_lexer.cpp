#include <gtest/gtest.h>
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "diagnostics/diagnostics.hpp"

using namespace::frontend;

TEST(lexerTest, Get) {
    ErrorReporter errors;
	Lexer lexer("int variable = 2 + 3", errors);
	Token token = lexer.get();
	ASSERT_EQ(token.type, TokenType::INT);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::IDENT);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::EQUAL);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::NUMBER_LIT);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::PLUS);
	token = lexer.get();
	ASSERT_EQ(token.type, TokenType::NUMBER_LIT);
}
