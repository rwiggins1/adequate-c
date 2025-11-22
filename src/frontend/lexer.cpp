#include "lexer.hpp"
#include "token.hpp"
#include <cctype>
#include <cstddef>
#include <string>

namespace frontend {
Lexer::Lexer(std::string src )
	: source(std::move(src)), position(0), line(1), column(1) {
	
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
	current = (position < src_length)  ? source[position] : '\0';
}

char Lexer::peekNext() noexcept {
	return (position < src_length) ? source[position+1] : '\0';
}

void Lexer::skipWhitespace() noexcept {
	while (position < src_length && isspace(current)) {
		advance();
	}
}

void Lexer::skipSingleLineComment() noexcept {
	advance(); advance();

	while (current != '\n') {
		advance();
	}
	advance();
}

void Lexer::skipMultiLineComment() noexcept {
	advance(); advance();

	while (current != '*' && peekNext() != '/') {
		advance();
	}
	advance(); advance(); advance();
}

// Determines identifier
Token Lexer::identifier() {
	size_t startLine = line;
	size_t startColumn = column;

	std::string text;
	text+=current;
	advance();

	while(std::isalnum(current) || current == '_') {
		text+=current;
		advance();
	}
	
	auto it = keywords.find(text);
	if (it != keywords.end()) {
		return {it->second, text, startLine, startColumn};
	}
	return {TokenType::IDENT, text, startLine, startColumn};
}

// Determines number
Token Lexer::number() {
	size_t startLine = line;
	size_t startColumn = column;


	std::string text;
	text+= current;
	advance();

	while(std::isdigit(current)) {
		text+=current;
		advance();
	}

	// check for float or double
	if (current == '.' && std::isdigit(peekNext())) {
		text+=current;
		advance();
		while (std::isdigit(current)) {
			text+=current;
			advance();
		}
	}

	return {TokenType::NUMBER, text, startLine, startColumn};
}

// Determine string literal
Token Lexer::string_lit() {
	size_t startLine = line;
	size_t startColumn = column;

	std::string text;
	text += current; // add open qoutes
	advance();

	while (current != '"' && position < src_length) {
		text+=current;
		advance();
	}
	text+=current; // add closing qoutes
	advance();
	return {TokenType::STRING_LIT, text, startLine, startColumn};
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
	if (position >= src_length){
		return {TokenType::T_EOF, "\0", line, column};
	}
	
	skipWhitespace();

	if (current == '/' && peekNext() == '/') {
		Lexer::skipSingleLineComment();
	} 
	else if (current == '/' && peekNext() == '*') {
		Lexer::skipMultiLineComment();
	}

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
			return {TokenType::MINUS, "-", line, column};
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
			return {TokenType::ASSIGN, "=", line, column};
		case '!':
			advance();
			if (current == '=') {
				return makeToken(TokenType::NOT_EQUAL, "!=");
			}
			return {TokenType::NOT, "!", line, column};
		case '>':
			advance();
			if (current == '=') {
				return makeToken(TokenType::GREATER_EQUAL, ">=");
			}
			return {TokenType::GREATER, ">", line, column};
		case '<':
			advance();
			if (current == '=') {
				return makeToken(TokenType::LESS_EQUAL, "<");
			}
			return {TokenType::LESS, "<", line, column};
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
	return tokenize();
}

}

