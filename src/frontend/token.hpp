#pragma once

#include <string>

namespace frontend {
enum class TokenType {
    // Keywords
    FUNC, INFER, IF, ELSE, WHILE, RETURN,
    
    // Types
    INT, FLOAT, CHAR, BOOL, VOID, STRING,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    EQUAL, NOT_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
    ASSIGN,
    AND, OR, NOT,
    ARROW,
    
    // Delimiters
    COLON, SEMICOLON, COMMA,
    LBRACE, RBRACE, LPAREN, RPAREN,
    
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
    
    Token(TokenType t, const std::string& lex, int ln, int col)
        : type(t), lexeme(lex), line(ln), column(col) {}
};
}

