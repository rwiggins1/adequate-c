#include "frontend/lexer/lexer.hpp"
#include "frontend/parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace frontend;

void testFile(const std::string &filename) {
	std::cout << "\n=== Testing Parser: " << filename << " ===\n";

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "ERROR: Could not open file: " << filename << "\n";
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string source = buffer.str();

	std::cout << "Source:\n" << source << "\n\n";

	Lexer lexer(source);
	Parser parser(lexer);

	auto varDecl = parser.parseVarDecl();

	if (varDecl) {
		std::cout << "Successfully parsed variable declaration!\n";
	}
	else {
		std::cout << "Failed to parse variable declaration\n";
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <filename>\n";
		return 1;
	}

	testFile(argv[1]);
	return 0;
}
