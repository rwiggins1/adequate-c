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

	std::unordered_map<std::string, TokenType> keywords;

	void advance() noexcept;
	char peekNext() noexcept;
	void skipWhitespace() noexcept;

public:
	Lexer(const std::string& src);

	Token getToken();
	Token peek();
};

}

