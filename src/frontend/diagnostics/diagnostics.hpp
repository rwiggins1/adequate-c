#pragma once
#include <string>
#include <vector>
#include <iostream>

namespace frontend {

enum class ErrorPhase { LEXER, PARSER, SEMANTIC, CODEGEN };

struct CompilerError {
	ErrorPhase phase;
	std::string message;
	std::string filename;
	size_t line;
	size_t column;
	bool isWarning;

	CompilerError(ErrorPhase p, std::string msg, std::string file,
		      size_t ln, size_t col, bool warn = false)
	    : phase(p), message(std::move(msg)), filename(std::move(file)),
	      line(ln), column(col), isWarning(warn) {}
};

class ErrorReporter {
	std::vector<CompilerError> errors;
	std::string currentFilename;
	size_t errorCount = 0;
	size_t warningCount = 0;

public:
	void setFilename(const std::string &filename) {
		currentFilename = filename;
	}

	void error(const std::string &message, size_t line, size_t column,
		   ErrorPhase phase = ErrorPhase::PARSER) {
		errors.emplace_back(phase, message, currentFilename, line,
				    column, false);
		errorCount++;
	}

	void warning(const std::string &message, size_t line, size_t column,
		     ErrorPhase phase = ErrorPhase::PARSER) {
		errors.emplace_back(phase, message, currentFilename, line,
				    column, true);
		warningCount++;
	}

	bool hasErrors() const { return errorCount > 0; }
	size_t getErrorCount() const { return errorCount; }
	size_t getWarningCount() const { return warningCount; }

	void printAll() const {
		for (const auto &err : errors) {
			std::cerr << err.filename << ":" << err.line << ":"
				  << err.column << ": ";

			if (err.isWarning) {
				std::cerr << "warning: ";
			}
			else {
				std::cerr << "error: ";
			}

			std::cerr << err.message << "\n";
		}

		if (errorCount > 0 || warningCount > 0) {
			std::cerr << "\nCompilation ";
			if (errorCount > 0) {
				std::cerr << "failed with " << errorCount
					  << " error(s)";
				if (warningCount > 0) {
					std::cerr << ", " << warningCount
						  << " warning(s)";
				}
			}
			else {
				std::cerr << "succeeded with " << warningCount
					  << " warning(s)";
			}
			std::cerr << "\n";
		}
	}

	const std::vector<CompilerError> &getErrors() const { return errors; }

	void clear() {
		errors.clear();
		errorCount = 0;
		warningCount = 0;
	}
};

} // namespace frontend
