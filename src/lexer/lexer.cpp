#include "lexer.hpp"
#include "diagnostics/diagnostics.hpp"
#include "token.hpp"
#include <cctype>
#include <cstddef>
#include <iterator>
#include <string>

namespace frontend {
Lexer::Lexer(std::string src, ErrorReporter& errors)
    : source(std::move(src)), errors(errors), src_length(source.length()), position(0), line(1),
      column(1), current(source.empty() ? '\0' : source[0]) {}

void Lexer::advance() noexcept {
	if (current == '\n') {
		line++;
		column = 1;
	}
	else {
		column++;
	}

	position++;
	current = (position < src_length) ? source[position] : '\0';
}

char Lexer::peekNext() noexcept {
	return (position < src_length) ? source[position + 1] : '\0';
}

void Lexer::skipWhitespace() noexcept {
	while (position < src_length &&
	       isspace(static_cast<unsigned char>(current)) != 0) {
		advance();
	}
}

void Lexer::skipSingleLineComment() noexcept {
	advance(); // consume '/'
	advance(); // consume '/'

	while (position < src_length && current != '\n') {
		advance();
	}

	if (current == '\n') {
		advance();
	}
}

void Lexer::skipMultiLineComment() {
    size_t start_line = line;
    size_t start_column = column;

	advance(); // consume '/'
	advance(); // consume '*'

	while (position < src_length &&
	       !(current == '*' && peekNext() == '/')) {
		advance();
	}

	if (position < src_length) {
		advance(); // consume '*'
		advance(); // consume '/'
	}
	else {
    	errors.error("unterminated block comment",
                    start_line, start_column, ErrorPhase::LEXER);
	}
}

// Skip whitespace and comments in alternation
void Lexer::skipTrivia() {
	while (true) {
		skipWhitespace();

		if (current == '/' && peekNext() == '/') {
			skipSingleLineComment();
		}
		else if (current == '/' && peekNext() == '*') {
			skipMultiLineComment();
		}
		else {
			break;
		}
	}
}

// Determines identifier
Token Lexer::identifier() {
	size_t startLine = line;
	size_t startColumn = column;

	std::string text;
	text += current;
	advance();

	while (std::isalnum(static_cast<unsigned char>(current)) != 0 ||
	       current == '_') {
		text += current;
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
	text += current;
	advance();

	while (std::isdigit(static_cast<unsigned char>(current)) != 0) {
		text += current;
		advance();
	}

	// check for float or double
	if (current == '.' &&
	    std::isdigit(static_cast<unsigned char>(peekNext())) != 0) {
		text += current;
		advance();
		while (std::isdigit(static_cast<unsigned char>(current)) != 0) {
			text += current;
			advance();
		}
	}

	return {TokenType::NUMBER, text, startLine, startColumn};
}

// Determine char literal
Token Lexer::char_lit() {
	size_t startLine = line;
	size_t startColumn = column;

	std::string text;
	text += current; // Opening '
	advance();

	// Regular character
	if (current != '\'' && position < src_length) {
		text += current;
		advance();
	}

	// Check for more than one character
	if (current != '\'') {
		while (current != '\'' && position < src_length) {
			text += current;
			advance();
		}
		if (current == '\'') {
			text += current;
			advance();
			errors.error("character literal must contain exactly one character",
                 startLine, startColumn, ErrorPhase::LEXER);
			return {TokenType::INVALID, text, startLine, startColumn};
		}
		errors.error("unterminated char literal",
                 startLine, startColumn, ErrorPhase::LEXER);
		return {TokenType::INVALID, text, startLine, startColumn};
	}

	text += current; // Closing '
	advance();

	return {TokenType::CHAR_LIT, text, startLine, startColumn};
}

// Determine string literal
Token Lexer::string_lit() {
	size_t startLine = line;
	size_t startColumn = column;

	std::string text;
	text += current; // add open qoutes
	advance();

	while (current != '"' && position < src_length) {
		text += current;
		advance();
	}
	if (position >= src_length) {
        errors.error("unterminated string literal", startLine, startColumn, ErrorPhase::LEXER);
        return {TokenType::INVALID, text, startLine, startColumn};
    }
	text += current; // add closing qoutes
	advance();
	return {TokenType::STRING_LIT, text, startLine, startColumn};
}

// Constructs token
Token Lexer::makeToken(TokenType type, const std::string &lexme, size_t start_column) {
	Token token(type, lexme, line, start_column);
	advance();
	return token;
}

// Tokenizer
Token Lexer::tokenize() {
	size_t start_line = line;
	size_t start_column = column;

	if (std::isalpha(static_cast<unsigned char>(current)) != 0) {
		return identifier();
	}

	if (std::isdigit(static_cast<unsigned char>(current)) != 0) {
		return number();
	}

	if (current == '\'') {
		return char_lit();
	}

	if (current == '"') {
		return string_lit();
	}

	switch (current) {
	// Operators
	case '+':
		advance();
		if (current == '+') {
			return makeToken(TokenType::PLUS_PLUS, "++", start_column);
		}
		if (current == '=') {
			return makeToken(TokenType::PLUS_ASSIGN, "+=", start_column);
		}
		return {TokenType::PLUS, "+", start_line, start_column};
	case '-':
		advance();
		if (current == '-') {
			return makeToken(TokenType::MINUS_MINUS, "--", start_column);
		}
		if (current == '=') {
			return makeToken(TokenType::MINUS_ASSIGN, "-=", start_column);
		}
		if (current == '>') {
			return makeToken(TokenType::ARROW, "->", start_column);
		}
		return {TokenType::MINUS, "-", start_line, start_column};
	case '*':
		advance();
		if (current == '=') {
			return makeToken(TokenType::MULTIPLY_ASSIGN, "*=", start_column);
		}
		return {TokenType::MULTIPLY, "*", start_line, start_column};
	case '/':
		advance();
		if (current == '=') {
			return makeToken(TokenType::DIVIDE_ASSIGN, "/=", start_column);
		}
		return {TokenType::DIVIDE, "/", start_line, start_column};
	case '%':
		advance();
		if (current == '=') {
			return makeToken(TokenType::MODULO_ASSIGN, "%=", start_column);
		}
		return {TokenType::MODULO, "%", start_line, start_column};
	case '=':
		advance();
		if (current == '=') {
			return makeToken(TokenType::EQUAL, "==", start_column);
		}
		return {TokenType::ASSIGN, "=", start_line, start_column};
	case '!':
		advance();
		if (current == '=') {
			return makeToken(TokenType::NOT_EQUAL, "!=", start_column);
		}
		return {TokenType::NOT, "!", start_line, start_column};
	case '>':
		advance();
		if (current == '=') {
			return makeToken(TokenType::GREATER_EQUAL, ">=", start_column);
		}
		if (current == '>') {
			advance();
			if (current == '=') {
				return makeToken(TokenType::RIGHT_SHIFT_ASSIGN,
						 ">>=", start_column);
			}
			return {TokenType::RIGHT_SHIFT, ">>", start_line,
				start_column};
		}
		return {TokenType::GREATER, ">", start_line, start_column};
	case '<':
		advance();
		if (current == '=') {
			return makeToken(TokenType::LESS_EQUAL, "<=", start_column);
		}
		if (current == '<') {
			advance();
			if (current == '=') {
				return makeToken(TokenType::LEFT_SHIFT_ASSIGN,
						 "<<=", start_column);
			}
			return {TokenType::LEFT_SHIFT, "<<", start_line,
				start_column};
		}
		return {TokenType::LESS, "<", start_line, start_column};
	case '&':
		advance();
		if (current == '&') {
			return makeToken(TokenType::AND, "&&", start_column);
		}
		if (current == '=') {
			return makeToken(TokenType::BIT_AND_ASSIGN, "&=", start_column);
		}
		return {TokenType::BIT_AND, "&", start_line, start_column};
	case '|':
		advance();
		if (current == '|') {
			return makeToken(TokenType::OR, "||", start_column);
		}
		if (current == '=') {
			return makeToken(TokenType::BIT_OR_ASSIGN, "|=", start_column);
		}
		return {TokenType::BIT_OR, "|", start_line, start_column};
	case '^':
		advance();
		if (current == '=') {
			return makeToken(TokenType::BIT_XOR_ASSIGN, "^=", start_column);
		}
		return {TokenType::BIT_XOR, "^", start_line, start_column};
	case '~':
		return makeToken(TokenType::BIT_NOT, "~", start_column);
	case '@':
		return makeToken(TokenType::REFERENCE, "@", start_column);
	case '.':
		return makeToken(TokenType::DOT, ".", start_column);
	case '?':
		return makeToken(TokenType::QUESTION, "?", start_column);
	// Delimiters
	case ':':
		advance();
		if (current == ':') {
			return makeToken(TokenType::SCOPE, "::", start_column);
		}
		return {TokenType::COLON, ":", start_line, start_column};
	case ';':
		return makeToken(TokenType::SEMICOLON, ";", start_column);
	case ',':
		return makeToken(TokenType::COMMA, ",", start_column);
	case '[':
		return makeToken(TokenType::LSQRBRACKET, "[", start_column);
	case ']':
		return makeToken(TokenType::RSQRBRACKET, "]", start_column);
	case '(':
		return makeToken(TokenType::OPAREN, "(", start_column);
	case ')':
		return makeToken(TokenType::CPAREN, ")", start_column);
	case '{':
		return makeToken(TokenType::LBRACE, "{", start_column);
	case '}':
		return makeToken(TokenType::RBRACE, "}", start_column);
	default:
    	errors.error("unexpected character '" + std::string(1, current) + "'",
                    line, column, ErrorPhase::LEXER);
		return makeToken(TokenType::INVALID, std::string(1, current), start_column);
	}
}

Token Lexer::get() {
	skipTrivia();
	if (position >= src_length) {
		return {TokenType::T_EOF, "\0", line, column};
	}
	return tokenize();
}

} // namespace frontend
