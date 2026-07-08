#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace ::frontend;

namespace {
// Lex the whole source, stopping at EOF
std::vector<Token> lexAll(const std::string &src, ErrorReporter &errors) {
	Lexer lexer(src, errors);
	std::vector<Token> tokens;
	while (true) {
		Token token = lexer.get();
		if (token.type == TokenType::T_EOF) {
			break;
		}
		tokens.push_back(token);
	}
	return tokens;
}
} // namespace

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

TEST(lexerTest, Keywords) {
	ErrorReporter errors;
	auto tokens = lexAll(
	    "func var infer if else for do while return break continue "
	    "switch case default struct enum const static namespace import "
	    "int float double char bool void string true false",
	    errors);

	std::vector<TokenType> expected = {
	    TokenType::FUNC,	  TokenType::VAR,      TokenType::INFER,
	    TokenType::IF,	  TokenType::ELSE,     TokenType::FOR,
	    TokenType::DO,	  TokenType::WHILE,    TokenType::RETURN,
	    TokenType::BREAK,	  TokenType::CONTINUE, TokenType::SWITCH,
	    TokenType::CASE,	  TokenType::DEFAULT,  TokenType::STRUCT,
	    TokenType::ENUM,	  TokenType::CONST,    TokenType::STATIC,
	    TokenType::NAMESPACE, TokenType::IMPORT,   TokenType::INT,
	    TokenType::FLOAT,	  TokenType::DOUBLE,   TokenType::CHAR,
	    TokenType::BOOL,	  TokenType::VOID,     TokenType::STRING,
	    TokenType::TRUE,	  TokenType::FALSE};

	ASSERT_EQ(tokens.size(), expected.size());
	for (size_t i = 0; i < expected.size(); i++) {
		EXPECT_EQ(tokens[i].type, expected[i])
		    << "token #" << i << " lexeme: " << tokens[i].lexeme;
	}
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, SingleCharOperatorsAndDelimiters) {
	ErrorReporter errors;
	auto tokens =
	    lexAll("+ - * / % = ! > < & | ^ ~ @ . ? : ; , [ ] ( ) { }", errors);

	std::vector<TokenType> expected = {
	    TokenType::PLUS,	    TokenType::MINUS,	 TokenType::STAR,
	    TokenType::SLASH,	    TokenType::PERCENT,	 TokenType::EQUAL,
	    TokenType::EXCLAMATION, TokenType::GREATER,	 TokenType::LESS,
	    TokenType::AMPERSAND,   TokenType::PIPE,	 TokenType::CARET,
	    TokenType::TILDE,	    TokenType::AT,	 TokenType::DOT,
	    TokenType::QUESTION,    TokenType::COLON,	 TokenType::SEMICOLON,
	    TokenType::COMMA,	    TokenType::LBRACKET, TokenType::RBRACKET,
	    TokenType::LPAREN,	    TokenType::RPAREN,	 TokenType::LBRACE,
	    TokenType::RBRACE};

	ASSERT_EQ(tokens.size(), expected.size());
	for (size_t i = 0; i < expected.size(); i++) {
		EXPECT_EQ(tokens[i].type, expected[i])
		    << "token #" << i << " lexeme: " << tokens[i].lexeme;
	}
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, MultiCharOperators) {
	ErrorReporter errors;
	auto tokens = lexAll("++ -- += -= -> *= /= %= == != >= <= >> << >>= "
			     "<<= && || &= |= ^= ::",
			     errors);

	std::vector<std::pair<TokenType, std::string>> expected = {
	    {TokenType::PLUS_PLUS, "++"},
	    {TokenType::MINUS_MINUS, "--"},
	    {TokenType::PLUS_EQUAL, "+="},
	    {TokenType::MINUS_EQUAL, "-="},
	    {TokenType::ARROW, "->"},
	    {TokenType::STAR_EQUAL, "*="},
	    {TokenType::SLASH_EQUAL, "/="},
	    {TokenType::PERCENT_EQUAL, "%="},
	    {TokenType::EQUAL_EQUAL, "=="},
	    {TokenType::EXCLAMATION_EQUAL, "!="},
	    {TokenType::GREATER_EQUAL, ">="},
	    {TokenType::LESS_EQUAL, "<="},
	    {TokenType::GREATER_GREATER, ">>"},
	    {TokenType::LESS_LESS, "<<"},
	    {TokenType::GREATER_GREATER_EQUAL, ">>="},
	    {TokenType::LESS_LESS_EQUAL, "<<="},
	    {TokenType::AMPERSAND_AMPERSAND, "&&"},
	    {TokenType::PIPE_PIPE, "||"},
	    {TokenType::AMPERSAND_EQUAL, "&="},
	    {TokenType::PIPE_EQUAL, "|="},
	    {TokenType::CARET_EQUAL, "^="},
	    {TokenType::COLON_COLON, "::"}};

	ASSERT_EQ(tokens.size(), expected.size());
	for (size_t i = 0; i < expected.size(); i++) {
		EXPECT_EQ(tokens[i].type, expected[i].first)
		    << "token #" << i << " lexeme: " << tokens[i].lexeme;
		EXPECT_EQ(tokens[i].lexeme, expected[i].second);
	}
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, Identifiers) {
	ErrorReporter errors;
	auto tokens = lexAll("x foo bar42 snake_case forx", errors);

	ASSERT_EQ(tokens.size(), 5);
	for (const auto &token : tokens) {
		EXPECT_EQ(token.type, TokenType::IDENT);
	}
	EXPECT_EQ(tokens[0].lexeme, "x");
	EXPECT_EQ(tokens[1].lexeme, "foo");
	EXPECT_EQ(tokens[2].lexeme, "bar42");
	EXPECT_EQ(tokens[3].lexeme, "snake_case");
	// keyword prefix must not be treated as a keyword
	EXPECT_EQ(tokens[4].lexeme, "forx");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, NumberLiterals) {
	ErrorReporter errors;
	auto tokens = lexAll("42 3.14 0 7.", errors);

	ASSERT_EQ(tokens.size(), 5);
	EXPECT_EQ(tokens[0].type, TokenType::NUMBER_LIT);
	EXPECT_EQ(tokens[0].lexeme, "42");
	EXPECT_EQ(tokens[1].type, TokenType::NUMBER_LIT);
	EXPECT_EQ(tokens[1].lexeme, "3.14");
	EXPECT_EQ(tokens[2].type, TokenType::NUMBER_LIT);
	EXPECT_EQ(tokens[2].lexeme, "0");
	// "7." is a number followed by a dot: the '.' only joins the
	// literal when followed by a digit
	EXPECT_EQ(tokens[3].type, TokenType::NUMBER_LIT);
	EXPECT_EQ(tokens[3].lexeme, "7");
	EXPECT_EQ(tokens[4].type, TokenType::DOT);
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, StringLiteral) {
	ErrorReporter errors;
	auto tokens = lexAll("\"Hello, World!\"", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::STRING_LIT);
	EXPECT_EQ(tokens[0].lexeme, "\"Hello, World!\"");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, UnterminatedStringLiteral) {
	ErrorReporter errors;
	auto tokens = lexAll("\"abc", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::INVALID);
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_EQ(errors.getErrors()[0].phase, ErrorPhase::LEXER);
}

TEST(lexerTest, CharLiteral) {
	ErrorReporter errors;
	auto tokens = lexAll("'a'", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::CHAR_LIT);
	EXPECT_EQ(tokens[0].lexeme, "'a'");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, MultiCharCharLiteral) {
	ErrorReporter errors;
	auto tokens = lexAll("'ab'", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::INVALID);
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_EQ(errors.getErrors()[0].phase, ErrorPhase::LEXER);
}

TEST(lexerTest, UnterminatedCharLiteral) {
	ErrorReporter errors;
	auto tokens = lexAll("'a", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::INVALID);
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_EQ(errors.getErrors()[0].phase, ErrorPhase::LEXER);
}

TEST(lexerTest, SingleLineComment) {
	ErrorReporter errors;
	auto tokens = lexAll("// a comment\nx", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::IDENT);
	EXPECT_EQ(tokens[0].lexeme, "x");
	EXPECT_EQ(tokens[0].line, 2);
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, MultiLineComment) {
	ErrorReporter errors;
	auto tokens = lexAll("/* first\nsecond */ x", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::IDENT);
	EXPECT_EQ(tokens[0].lexeme, "x");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, CommentBetweenTokens) {
	ErrorReporter errors;
	auto tokens = lexAll("1 /* mid */ + // trailing\n 2", errors);

	ASSERT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, TokenType::NUMBER_LIT);
	EXPECT_EQ(tokens[1].type, TokenType::PLUS);
	EXPECT_EQ(tokens[2].type, TokenType::NUMBER_LIT);
	EXPECT_FALSE(errors.hasErrors());
}

TEST(lexerTest, UnterminatedBlockComment) {
	ErrorReporter errors;
	auto tokens = lexAll("/* never closed", errors);

	EXPECT_TRUE(tokens.empty());
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_EQ(errors.getErrors()[0].phase, ErrorPhase::LEXER);
}

TEST(lexerTest, LineAndColumnTracking) {
	ErrorReporter errors;
	auto tokens = lexAll("ab cd\n  ef", errors);

	ASSERT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].line, 1);
	EXPECT_EQ(tokens[0].column, 1);
	EXPECT_EQ(tokens[1].line, 1);
	EXPECT_EQ(tokens[1].column, 4);
	EXPECT_EQ(tokens[2].line, 2);
	EXPECT_EQ(tokens[2].column, 3);
}

TEST(lexerTest, UnexpectedCharacter) {
	ErrorReporter errors;
	auto tokens = lexAll("#", errors);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].type, TokenType::INVALID);
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_EQ(errors.getErrors()[0].phase, ErrorPhase::LEXER);
}

TEST(lexerTest, EmptyAndWhitespaceOnlySource) {
	{
		ErrorReporter errors;
		Lexer lexer("", errors);
		EXPECT_EQ(lexer.get().type, TokenType::T_EOF);
	}
	{
		ErrorReporter errors;
		Lexer lexer("  \t\n  ", errors);
		EXPECT_EQ(lexer.get().type, TokenType::T_EOF);
	}
}

TEST(lexerTest, EofIsSticky) {
	ErrorReporter errors;
	Lexer lexer("x", errors);
	EXPECT_EQ(lexer.get().type, TokenType::IDENT);
	EXPECT_EQ(lexer.get().type, TokenType::T_EOF);
	EXPECT_EQ(lexer.get().type, TokenType::T_EOF);
}
