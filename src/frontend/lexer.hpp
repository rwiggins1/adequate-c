#pragma once

#include <string>
#include <unordered_map>
#include "token.hpp"

namespace frontend {
class Lexer {
private:
	std::string source; // source code
	size_t src_length = source.length();
	size_t position;
	size_t line;
	size_t column;
	char current;

	// Initialize keyword map
	static inline const std::unordered_map<std::string, TokenType> KEYWORDS = {
		{"func", TokenType::FUNC},
		{"infer", TokenType::INFER},
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"for", TokenType::FOR},
		{"do", TokenType::DO},
		{"while", TokenType::WHILE},
		{"return", TokenType::RETURN},
		{"break", TokenType::BREAK},
		{"struct", TokenType::STRUCT},
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
	void skipSingleLineComment() noexcept;
	void skipMultiLineComment() noexcept;

	Token identifier();
	Token number();
	Token string_lit();

	Token makeToken(TokenType type, const std::string& lexme);
	Token tokenize();

public:
	Lexer(std::string src);
	
	Token peek();
	Token get();
};

}

