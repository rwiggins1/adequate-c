#pragma once

#include <cstdint>
#include <string>

namespace frontend {
enum class TokenType : std::uint8_t {
	// Keywords
	FUNC, INFER, IF, ELSE, FOR, DO, WHILE, RETURN, BREAK,
	SWITCH, CASE, DEFAULT,
	STRUCT, CLASS, ENUM,
	STATIC, PUBLIC, PRIVATE,
	TRY, CATCH, THROW,
	NAMESPACE, OPERATOR,
	IMPORT,
    
	// Types
	INT, FLOAT, DOUBLE, CHAR, BOOL, VOID, STRING,
    
	// Operators
	PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
	PLUS_PLUS, MINUS_MINUS,  // ++ --
	PLUS_ASSIGN, MINUS_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN,  // += -= *= /=

	EQUAL, NOT_EQUAL,
	GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
	ASSIGN,

	AND, OR, NOT,
	BIT_AND, BIT_OR, BIT_XOR, BIT_NOT,
	LEFT_SHIFT, RIGHT_SHIFT,

	ARROW, REFERENCE,
	DOT,
	QUESTION, // ? (for ternary)
    
	// Delimiters
	COLON, SEMICOLON, COMMA,
	LBRACE, RBRACE, OPAREN, CPAREN, RSQRBRACKET, LSQRBRACKET,
    
	// Literals
	NUMBER, STRING_LIT, CHAR_LIT, IDENT,
	TRUE, FALSE,
    
	// Special
	T_EOF,
	INVALID
};

struct Token {
	TokenType type;
	std::string lexeme;
	size_t line;
	size_t column;
    
	Token(TokenType t, std::string lex, size_t ln, size_t col)
		: type(t), lexeme(std::move(lex)), line(ln), column(col) {}
};
}

