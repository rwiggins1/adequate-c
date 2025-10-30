#include "lexer.hpp"
#include "token.hpp"
#include <cctype>
#include <string>
#include <iostream>

namespace frontend {
Lexer::Lexer(const std::string& src )
	: source(src), position(0), line(1), column(1) {
	// Initialize keyword map
        keywords["func"] = TokenType::FUNC;
        keywords["infer"] = TokenType::INFER;
        keywords["if"] = TokenType::IF;
        keywords["else"] = TokenType::ELSE;
        keywords["while"] = TokenType::WHILE;
        keywords["return"] = TokenType::RETURN;
        keywords["int"] = TokenType::INT;
        keywords["float"] = TokenType::FLOAT;
        keywords["bool"] = TokenType::BOOL;
        keywords["void"] = TokenType::VOID;
        keywords["string"] = TokenType::STRING;
        keywords["true"] = TokenType::TRUE;
        keywords["false"] = TokenType::FALSE;
	
	// Set current to first character
	current = source.empty() ? '\0' : source[0];
} 

void Lexer::advance() noexcept {
	if (current == '\n') {
		line++;
		column = 1;
	}
	else {
		column++;
	}

	position++;
	current = (position < source.length()) ? source[position] : '\0';
}

char Lexer::peekNext() noexcept {
	return (position < source.length()) ? source[position] : '\0';
}

void Lexer::skipWhitespace() noexcept {
	while (position < source.length() && isspace(current)) {
		advance();
	}
}

// Determines identifier
Token Lexer::identifier() {
	int startLine = line;
	int startColumn = column;

	std::string text;
	text+=current;
	advance();

	while(std::isalnum(current)) {
		text+=current;
		advance();
	}
	
	auto it = keywords.find(text);
	if (it != keywords.end()) {
		return Token(it->second, text, startLine, startColumn);
	}
	return Token(TokenType::IDENT, text, startLine, startColumn);
}

// Determines number
Token Lexer::number() {
	int startLine = line;
	int startColumn = column;


	std::string text;
	text+= current;
	advance();

	while(std::isdigit(current)) {
		text+=current;
		advance();
	}
	return Token(TokenType::NUMBER, text, startLine, startColumn);
}

// Determine string literal
Token Lexer::string_lit() {
	int startLine = line;
	int startColumn = column;

	std::string text;
	text += current; // add open qoutes
	advance();

	while (current != '"' && position < source.length()) {
		text+=current;
		advance();
	}
	text+=current; // add closing qoutes
	advance();
	return Token(TokenType::STRING_LIT, text, startLine, startColumn);
}

// Constructs token
Token Lexer::makeToken(TokenType type, const std::string lexme) {
	Token token(type, lexme, line, column);
	advance();
	return token;
}

// Tokenizer
Token Lexer::tokenize() {
	//EOF
	if (position >= source.length()) {
		return Token(TokenType::T_EOF, "\0", line, column);
	}
	
	skipWhitespace();

	if (std::isalpha(current)) {
		return identifier();
	}

	if (std::isdigit(current)) {
		return number();
	}

	if (current == '"') {
		return string_lit();
	}

	switch (current) {
		// Operators
		case '+':
			return makeToken(TokenType::PLUS, "+");
		case '-':
			advance();
			if (current == '>') {
				return makeToken(TokenType::ARROW,"->");
			}	
			return Token(TokenType::MINUS, "-", line, column);
		case '*':
			return makeToken(TokenType::MULTIPLY, "*");
		case '/':
			return makeToken(TokenType::DIVIDE, "/");
		case '%':
			return makeToken(TokenType::MODULO, "%");
		case '=':
			advance();
			if (current == '=') {
				return makeToken(TokenType::EQUAL, "==");
			}
			return Token(TokenType::ASSIGN, "=", line, column);
		case '!':
			advance();
			if (current == '=') {
				return makeToken(TokenType::NOT_EQUAL, "!=");
			}
			return Token(TokenType::NOT, "!", line, column);
		case '>':
			advance();
			if (current == '=') {
				return makeToken(TokenType::GREATER_EQUAL, ">=");
			}
			return Token(TokenType::GREATER, ">", line, column);
		case '<':
			advance();
			if (current == '=') {
				return makeToken(TokenType::LESS_EQUAL, "<");
			}
			return Token(TokenType::LESS, "<", line, column);
		case '&':
			return makeToken(TokenType::AND, "&");
		case '|':
			return makeToken(TokenType::OR, "|");
		case '@':
			return makeToken(TokenType::REFERENCE, "@");
		// Delimiters
		case ':':
			return makeToken(TokenType::COLON, ":");
		case ';':
			return makeToken(TokenType::SEMICOLON, ";");
		case ',':
			return makeToken(TokenType::COMMA, ",");
		case '[':
			return makeToken(TokenType::LSQRBRACKET, "[");
		case ']':
			return makeToken(TokenType::RSQRBRACKET, "]");
		case '(':
			return makeToken(TokenType::LPAREN, "(");
		case ')':
			return makeToken(TokenType::RPAREN, ")");
		case '{':
			return makeToken(TokenType::LBRACE, "{");
		case '}':
			return makeToken(TokenType::RBRACE, "}");
		default:	
			return makeToken(TokenType::INVALID, std::string(1, current));
	}
}

Token Lexer::get() {
	Token token = tokenize();
	return token;
}

}

