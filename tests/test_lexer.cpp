#include <iostream>
#include <fstream>
#include <sstream>
#include "../src/frontend/lexer.hpp"
#include "../src/frontend/token.hpp"

using namespace frontend;

// Convert TokenType to a string for display
const char* tokenTypeToString(TokenType type) {
	switch (type) {
	case TokenType::FUNC: return "FUNC";
	case TokenType::INFER: return "INFER";
	case TokenType::IF: return "IF";
	case TokenType::ELSE: return "ELSE";
	case TokenType::WHILE: return "WHILE";
	case TokenType::RETURN: return "RETURN";
	case TokenType::INT: return "INT";
	case TokenType::FLOAT: return "FLOAT";
	case TokenType::BOOL: return "BOOL";
	case TokenType::VOID: return "VOID";
	case TokenType::STRING: return "STRING";
	case TokenType::TRUE: return "TRUE";
	case TokenType::FALSE: return "FALSE";
	case TokenType::PLUS: return "PLUS";
	case TokenType::MINUS: return "MINUS";
	case TokenType::MULTIPLY: return "MULTIPLY";
	case TokenType::DIVIDE: return "DIVIDE";
	case TokenType::MODULO: return "MODULO";
	case TokenType::EQUAL: return "EQUAL";
	case TokenType::NOT_EQUAL: return "NOT_EQUAL";
	case TokenType::GREATER: return "GREATER";
	case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
	case TokenType::LESS: return "LESS";
	case TokenType::LESS_EQUAL: return "LESS_EQUAL";
	case TokenType::ASSIGN: return "ASSIGN";
	case TokenType::AND: return "AND";
	case TokenType::OR: return "OR";
	case TokenType::NOT: return "NOT";
	case TokenType::ARROW: return "ARROW";
	case TokenType::COLON: return "COLON";
	case TokenType::SEMICOLON: return "SEMICOLON";
	case TokenType::COMMA: return "COMMA";
	case TokenType::LBRACE: return "LBRACE";
	case TokenType::RBRACE: return "RBRACE";
	case TokenType::LPAREN: return "LPAREN";
	case TokenType::RPAREN: return "RPAREN";
	case TokenType::NUMBER: return "NUMBER";
	case TokenType::STRING_LIT: return "STRING_LIT";
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
	
	// Tokenize
	Lexer lexer(source);
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
