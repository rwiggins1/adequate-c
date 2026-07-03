#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "diagnostics/diagnostics.hpp"

using namespace frontend;

// Convert TokenType to a string for display
const char* tokenTypeToString(TokenType type) {
	switch (type) {
	case TokenType::FUNC: return "FUNC";
	case TokenType::VAR: return "VAR";
	case TokenType::INFER: return "INFER";
	case TokenType::IF: return "IF";
	case TokenType::ELSE: return "ELSE";
	case TokenType::FOR: return "FOR";
	case TokenType::DO: return "DO";
	case TokenType::WHILE: return "WHILE";
	case TokenType::RETURN: return "RETURN";
	case TokenType::BREAK: return "BREAK";
	case TokenType::CONTINUE: return "CONTINUE";
	case TokenType::SWITCH: return "SWITCH";
	case TokenType::CASE: return "CASE";
	case TokenType::DEFAULT: return "DEFAULT";
	case TokenType::STRUCT: return "STRUCT";
	case TokenType::ENUM: return "ENUM";
	case TokenType::CONST: return "CONST";
	case TokenType::STATIC: return "STATIC";
	case TokenType::NAMESPACE: return "NAMESPACE";
	case TokenType::IMPORT: return "IMPORT";
	case TokenType::INT: return "INT";
	case TokenType::FLOAT: return "FLOAT";
	case TokenType::DOUBLE: return "DOUBLE";
	case TokenType::CHAR: return "CHAR";
	case TokenType::BOOL: return "BOOL";
	case TokenType::VOID: return "VOID";
	case TokenType::STRING: return "STRING";
	case TokenType::TRUE: return "TRUE";
	case TokenType::FALSE: return "FALSE";
	case TokenType::PLUS: return "PLUS";
	case TokenType::MINUS: return "MINUS";
	case TokenType::STAR: return "STAR";
	case TokenType::SLASH: return "SLASH";
	case TokenType::PERCENT: return "PERCENT";
	case TokenType::PLUS_PLUS: return "PLUS_PLUS";
	case TokenType::MINUS_MINUS: return "MINUS_MINUS";
	case TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
	case TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
	case TokenType::STAR_EQUAL: return "STAR_EQUAL";
	case TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
	case TokenType::PERCENT_EQUAL: return "PERCENT_EQUAL";
	case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
	case TokenType::EXCLAMATION_EQUAL: return "EXCLAMATION_EQUAL";
	case TokenType::GREATER: return "GREATER";
	case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
	case TokenType::LESS: return "LESS";
	case TokenType::LESS_EQUAL: return "LESS_EQUAL";
	case TokenType::EQUAL: return "EQUAL";
	case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
	case TokenType::PIPE_PIPE: return "PIPE_PIPE";
	case TokenType::EXCLAMATION: return "EXCLAMATION";
	case TokenType::AMPERSAND: return "AMPERSAND";
	case TokenType::PIPE: return "PIPE";
	case TokenType::CARET: return "CARET";
	case TokenType::TILDE: return "TILDE";
	case TokenType::LESS_LESS: return "LESS_LESS";
	case TokenType::GREATER_GREATER: return "GREATER_GREATER";
	case TokenType::AMPERSAND_EQUAL: return "AMPERSAND_EQUAL";
	case TokenType::PIPE_EQUAL: return "PIPE_EQUAL";
	case TokenType::CARET_EQUAL: return "CARET_EQUAL";
	case TokenType::LESS_LESS_EQUAL: return "LESS_LESS_EQUAL";
	case TokenType::GREATER_GREATER_EQUAL: return "GREATER_GREATER_EQUAL";
	case TokenType::ARROW: return "ARROW";
	case TokenType::AT: return "AT";
	case TokenType::DOT: return "DOT";
	case TokenType::QUESTION: return "QUESTION";
	case TokenType::COLON_COLON: return "COLON_COLON";
	case TokenType::COLON: return "COLON";
	case TokenType::SEMICOLON: return "SEMICOLON";
	case TokenType::COMMA: return "COMMA";
	case TokenType::LBRACE: return "LBRACE";
	case TokenType::RBRACE: return "RBRACE";
	case TokenType::LPAREN: return "LPAREN";
	case TokenType::RPAREN: return "RPAREN";
	case TokenType::LBRACKET: return "LBRACKET";
	case TokenType::RBRACKET: return "RBRACKET";
	case TokenType::NUMBER_LIT: return "NUMBER_LIT";
	case TokenType::STRING_LIT: return "STRING_LIT";
	case TokenType::CHAR_LIT: return "CHAR_LIT";
	case TokenType::IDENT: return "IDENT";
	case TokenType::T_EOF: return "EOF";
	case TokenType::INVALID: return "INVALID";
	default: return "UNKNOWN";
	}
}

void testFile(const std::string& filename) {
	std::cout << "\n=== Testing: " << filename << " ===\n";

	// Read file
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "ERROR: Could not open file: " << filename << "\n";
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string source = buffer.str();

	std::cout << "Source:\n" << source << "\n\n";
	std::cout << "Tokens:\n";

	ErrorReporter error;

	// Tokenize
	Lexer lexer(source, error);
	Token token = lexer.get();

	while (token.type != TokenType::T_EOF) {
		std::cout << tokenTypeToString(token.type)
		          << "\t'" << token.lexeme << "'"
		          << "\t[" << token.line << ":" << token.column << "]\n";
		token = lexer.get();
	}

	std::cout << tokenTypeToString(token.type) << "\n";
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		return 1;
	}

	testFile(argv[1]);

	return 0;
}
