#include "lexer.hpp"

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
	while (position < source.length() && isspace(static_cast<unsigned char>(source[position]))) {
		advance();
	}
}

}

