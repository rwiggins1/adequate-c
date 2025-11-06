#pragma once

#include <string>
#include <unordered_map>
#include "token.hpp"

namespace frontend {
class Lexer {
private:
	std::string source; // source code
	size_t position;
	size_t line;
	size_t column;
	char current;

	// Initialize keyword map
	static inline const std::unordered_map<std::string, TokenType> keywords = {
		{"func", TokenType::FUNC},
		{"infer", TokenType::INFER},
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"while", TokenType::WHILE},
		{"return", TokenType::RETURN},
		{"int", TokenType::INT},
		{"float", TokenType::FLOAT},
		{"double", TokenType::DOUBLE},
		{"bool", TokenType::BOOL},
		{"void", TokenType::VOID},
		{"string", TokenType::STRING},
		{"true", TokenType::TRUE},
		{"false", TokenType::FALSE}
	};

	void advance() noexcept;
	char peekNext() noexcept;
	void skipWhitespace() noexcept;

	Token identifier();
	Token number();
	Token string_lit();

	Token makeToken(TokenType, const std::string);
	Token tokenize();

public:
	Lexer(const std::string& src);
	
	Token peek();
	Token get();
};

}

