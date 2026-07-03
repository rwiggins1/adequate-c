#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include "token.hpp"
#include "diagnostics/diagnostics.hpp"

namespace frontend {
class Lexer {
private:
	std::string source; // source code
	ErrorReporter& errors;
	size_t src_length;
	size_t position;
	size_t line;
	size_t column;
	char current;

	// Initialize keyword map
	static inline const std::unordered_map<std::string, TokenType> KEYWORDS = {
		{"func", TokenType::FUNC},
		{"var", TokenType::VAR},
		{"infer", TokenType::INFER},
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"for", TokenType::FOR},
		{"do", TokenType::DO},
		{"while", TokenType::WHILE},
		{"return", TokenType::RETURN},
		{"break", TokenType::BREAK},
		{"continue", TokenType::CONTINUE},

		{"switch", TokenType::SWITCH},
		{"case", TokenType::CASE},
		{"default", TokenType::DEFAULT},

		{"struct", TokenType::STRUCT},
		{"enum", TokenType::ENUM},

		{"const", TokenType::CONST},
		{"static", TokenType::STATIC},

		{"namespace", TokenType::NAMESPACE},
		{"import", TokenType::IMPORT},

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
	void skipMultiLineComment();
	void skipTrivia();

	Token identifier();
	Token number();
	Token string_lit();
	Token char_lit();

	Token makeToken(TokenType type, const std::string& lexme, size_t start_column);
	Token tokenize();

public:
	Lexer(std::string src, ErrorReporter& errors);

	Token peek();
	Token get();
};

}
