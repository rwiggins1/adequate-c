#include "lexer.hpp"
#include "token.hpp"
#include <cctype>
#include <cstddef>
#include <string>

namespace frontend {
Lexer::Lexer(std::string src )
	: source(std::move(src)), src_length(source.length()), position(0), line(1), column(1),
	current(source.empty() ? '\0' : source[0]) {
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
	while (position < src_length && isspace(static_cast<unsigned char>(current)) != 0) {
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

	while(std::isalnum(static_cast<unsigned char>(current)) != 0 || current == '_') {
		text+=current;
		advance();
	}
	
	auto it = KEYWORDS.find(text);
	if (it != KEYWORDS.end()) {
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

	while(std::isdigit(static_cast<unsigned char>(current)) != 0) {
		text+=current;
		advance();
	}

	// check for float or double
	if (current == '.' && std::isdigit(static_cast<unsigned char>(peekNext())) != 0) {
		text+=current;
		advance();
		while (std::isdigit(static_cast<unsigned char>(current)) != 0) {
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
Token Lexer::makeToken(TokenType type, const std::string& lexme) {
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

	if (std::isalpha(static_cast<unsigned char>(current)) != 0) {
		return identifier();
	}

	if (std::isdigit(static_cast<unsigned char>(current)) != 0) {
		return number();
	}

	if (current == '"') {
		return string_lit();
	}

	switch (current) {
		// Operators
		case '+':
			advance();
			if (current == '+') {
				return makeToken(TokenType::PLUS_PLUS, "++");
			}
			if (current == '=') {
				return makeToken(TokenType::PLUS_ASSIGN, "+=");
			}
			return {TokenType::PLUS, "+", line, column};
		case '-':
			advance();
			if (current == '-') {
				return makeToken(TokenType::MINUS_MINUS, "--");
			}
			if (current == '=') {
				return makeToken(TokenType::MINUS_ASSIGN, "-=");
			}
			if (current == '>') {
				return makeToken(TokenType::ARROW,"->");
			}	
			return {TokenType::MINUS, "-", line, column};
		case '*':
			advance();
			if (current == '=') {
				return makeToken(TokenType::MULTIPLY_ASSIGN, "*=");
			}
			return {TokenType::MULTIPLY, "*", line, column};
		case '/':
			advance();
			if (current == '=') {
				return makeToken(TokenType::DIVIDE_ASSIGN, "/=");
			}
			return {TokenType::DIVIDE, "/", line, column};
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
			advance();
			if (current == '&') {
				return makeToken(TokenType::AND, "&&");
			}
			return makeToken(TokenType::BIT_AND, "&");
		case '|':
			advance();
			if (current == '|') {
				return makeToken(TokenType::OR, "||");
			}
			return makeToken(TokenType::BIT_OR, "|");
		case '^':
			return makeToken(TokenType::BIT_XOR, "^");
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
			return makeToken(TokenType::OPAREN, "(");
		case ')':
			return makeToken(TokenType::CPAREN, ")");
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

