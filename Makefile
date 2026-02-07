# Configuration
BUILD_DIR := build
EXECUTABLE := adequatec
TEST_LEXER := test_lexer
TEST_AST := test_ast
SRC_DIR := src
TEST_DIR := tests

.PHONY: all build clean format run help

# Default target
all: build

# Build project
build:
	@echo "Building Adequate-C..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && $(MAKE) -j4
	@echo "Build complete! Executable: $(BUILD_DIR)/$(EXECUTABLE)"

# Clean build
clean:
	@echo "Cleaning $(BUILD_DIR) directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Format source code
format:
	@echo "Formatting source code in $(SRC_DIR)..."
	@find $(SRC_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
	@echo "Formatting complete!"

# Build and run
run: build
	@echo ""
	@echo "Running $(EXECUTABLE):"
	@echo "----------------------------"
	@./$(BUILD_DIR)/$(EXECUTABLE)

# Lint code with clang-tidy
lint:
	@echo "Running clang-tidy on source files..."
	@find $(SRC_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -p $(BUILD_DIR)
	@echo "Lint complete!"

# Test lexer
test-lexer: build
	@echo ""
	@echo "Testing Lexer:"
	@echo "=============="
	@./$(BUILD_DIR)/$(TEST_LEXER) $(TEST_DIR)/input/lexer/test_keywords.ac

# Test lexer
test-ast: build
	@echo ""
	@echo "Testing Abstract Syntax Tree:"
	@echo "=============="
	@./$(BUILD_DIR)/$(TEST_AST) $(TEST_DIR)/input/ast/variableDec.ac

# Google Tests
test-gtest: build
	@echo ""
	@echo "Running Google Tests:"
	@echo "===================="
	@cd $(BUILD_DIR) && ctest --output-on-failure

test-all: test-lexer test-ast test-gtest
	@echo ""
	@echo "All tests complete!"

# Show help
help:
	@echo "Adequate-C Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make build   - Build the project"
	@echo "  make clean   - Remove $(BUILD_DIR) directory"
	@echo "  make format  - Format all source code"
	@echo "  make run     - Build and run $(EXECUTABLE)"
	@echo "  make lint      - Run clang-tidy checks"
	@echo "  make help    - Show this help message"

.DEFAULT_GOAL := build
