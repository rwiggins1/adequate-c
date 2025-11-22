#pragma once

#include <string>

namespace frontend {
enum class TokenType {
    // Keywords
    FUNC, INFER, IF, ELSE, WHILE, RETURN,
    
    // Types
    INT, FLOAT, DOUBLE, CHAR, BOOL, VOID, STRING,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    EQUAL, NOT_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
    ASSIGN,
    AND, OR, NOT,
    ARROW, REFERENCE,
    
    // Delimiters
    COLON, SEMICOLON, COMMA,
    LBRACE, RBRACE, LPAREN, RPAREN, RSQRBRACKET, LSQRBRACKET,
    
    // Literals
    NUMBER, STRING_LIT, IDENT,
    TRUE, FALSE,
    
    // Special
    T_EOF,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& lex, size_t ln, size_t col)
        : type(t), lexeme(lex), line(ln), column(col) {}
};
}

