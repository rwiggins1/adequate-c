#pragma once

#include <cstdint>
#include <string>

namespace frontend {
enum class TokenType : std::uint8_t {
	// Keywords
	FUNC,
	VAR,
	INFER,
	IF,
	ELSE,
	FOR,
	DO,
	WHILE,
	RETURN,
	BREAK,
	CONTINUE,
	SWITCH,
	CASE,
	DEFAULT,
	STRUCT,
	ENUM,
	CONST,
	STATIC,
	NAMESPACE,
	IMPORT,

	// Types
	INT,
	FLOAT,
	DOUBLE,
	CHAR,
	BOOL,
	VOID,
	STRING,

	// Operators
	PLUS,	       // '+'
	MINUS,	       // '-'
	STAR,	       // '*'
	SLASH,	       // '/'
	PERCENT,       // '%'
	PLUS_PLUS,     // '++'
	MINUS_MINUS,   // '--'
	PLUS_EQUAL,    // '+='
	MINUS_EQUAL,   // '-='
	STAR_EQUAL,    // '*='
	SLASH_EQUAL,   // '/='
	PERCENT_EQUAL, // '%='

	EQUAL_EQUAL,	   // '=='
	EXCLAMATION_EQUAL, // '!='
	GREATER,	   // '>'
	GREATER_EQUAL,	   // '>='
	LESS,		   // '<'
	LESS_EQUAL,	   // '<='
	EQUAL,		   // '='

	AMPERSAND_AMPERSAND,   // '&&'
	PIPE_PIPE,	       // '||'
	EXCLAMATION,	       // '!'
	AMPERSAND,	       // '&'
	PIPE,		       // '|'
	CARET,		       // '^'
	TILDE,		       // '~'
	LESS_LESS,	       // '<<'
	GREATER_GREATER,       // '>>'
	AMPERSAND_EQUAL,       // '&='
	PIPE_EQUAL,	       // '|='
	CARET_EQUAL,	       // '^='
	LESS_LESS_EQUAL,       // '<<='
	GREATER_GREATER_EQUAL, // '>>='

	ARROW,	     // '->'
	AT,	     // '@'
	DOT,	     // '.'
	QUESTION,    // '?' (for ternary)
	COLON_COLON, // '::'

	// Delimiters
	COLON,	   // ':'
	SEMICOLON, // ';'
	COMMA,	   // ','
	LBRACE,	   // '{'
	RBRACE,	   // '}'
	LPAREN,	   // '('
	RPAREN,	   // ')'
	LBRACKET,  // '['
	RBRACKET,  // ']'

	// Literals
	NUMBER_LIT,
	STRING_LIT,
	CHAR_LIT,
	IDENT,
	TRUE,
	FALSE,

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
} // namespace frontend
