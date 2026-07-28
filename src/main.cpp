#include "ast/printer.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
void printUsage() { std::cerr << "usage: adq [--dump-ast] <filename>\n"; }
} // namespace

int main(int argc, char **argv) {
	bool dumpAst = false;
	const char *filename = nullptr;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--dump-ast") {
			dumpAst = true;
		}
		else if (filename == nullptr) {
			filename = argv[i];
		}
		else {
			printUsage();
			return 1;
		}
	}

	if (filename == nullptr) {
		printUsage();
		return 1;
	}

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "adq: cannot open file '" << filename << "'\n";
		return 1;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string source = buffer.str();

	frontend::ErrorReporter errors;
	errors.setFilename(filename);
	frontend::Lexer lexer(source, errors);
	frontend::Parser parser(lexer, errors);

	auto program = parser.parseProgram();

	if (errors.hasErrors() || program == nullptr) {
		errors.printAll();
		return 1;
	}

	if (dumpAst) {
		frontend::ast::ASTPrinter printer(std::cout);
		printer.print(*program);
	}
	return 0;
}
