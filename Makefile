# Configuration
BUILD_DIR := build
COVERAGE_DIR := build-coverage
SANITIZE_DIR := build-sanitize
EXECUTABLE := adequatec
TEST_LEXER := test_lexer
TEST_AST := test_ast
SRC_DIR := src
TEST_DIR := tests

# DIR selects which build directory the test targets run against. Overridden
# by `sanitize` (via sub-make) to point at $(SANITIZE_DIR) instead of $(BUILD_DIR).
DIR ?= $(BUILD_DIR)
BUILD_DEP := build
ifneq ($(DIR),$(BUILD_DIR))
BUILD_DEP :=
endif

.PHONY: all build clean format run help lint test-lexer test-ast test-gtest test-all coverage sanitize build-sanitize

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
	@echo "Cleaning $(BUILD_DIR), $(COVERAGE_DIR), and $(SANITIZE_DIR) directories..."
	@rm -rf $(BUILD_DIR) $(COVERAGE_DIR) $(SANITIZE_DIR)
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

# Test lexer (runs against $(DIR), default $(BUILD_DIR))
test-lexer: $(BUILD_DEP)
	@echo ""
	@echo "Testing Lexer:"
	@echo "=============="
	@./$(DIR)/$(TEST_LEXER) $(TEST_DIR)/input/lexer/test_keywords.ac

# Test ast (runs against $(DIR), default $(BUILD_DIR))
test-ast: $(BUILD_DEP)
	@echo ""
	@echo "Testing Abstract Syntax Tree:"
	@echo "=============="
	@./$(DIR)/$(TEST_AST) $(TEST_DIR)/input/ast/variableDec.ac

# Google Tests (runs against $(DIR), default $(BUILD_DIR))
test-gtest: $(BUILD_DEP)
	@echo ""
	@echo "Running Google Tests:"
	@echo "===================="
	@cd $(DIR) && ctest --output-on-failure

test-all: test-lexer test-ast test-gtest
	@echo ""
	@echo "All tests complete!"

# Build with --coverage, run tests, and generate a gcovr report (requires gcovr)
coverage:
	@echo "Building with coverage instrumentation..."
	@mkdir -p $(COVERAGE_DIR)
	@cd $(COVERAGE_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DADQ_ENABLE_COVERAGE=ON .. && $(MAKE) -j4 coverage
	@echo "Coverage report: $(COVERAGE_DIR)/coverage/index.html"

# Build with ASan + UBSan (without running tests)
build-sanitize:
	@echo "Building with ASan/UBSan instrumentation..."
	@mkdir -p $(SANITIZE_DIR)
	@cd $(SANITIZE_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DADQ_ENABLE_ASAN=ON -DADQ_ENABLE_UBSAN=ON .. && $(MAKE) -j4

# Build with ASan + UBSan and run all tests against that build
sanitize: build-sanitize
	@$(MAKE) --no-print-directory test-lexer test-ast test-gtest DIR=$(SANITIZE_DIR)
	@echo ""
	@echo "All sanitizer tests complete!"

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
	@echo "  make coverage  - Build with --coverage, run tests, generate gcovr report"
	@echo "  make sanitize  - Build with ASan+UBSan and run all tests"
	@echo "  make help    - Show this help message"

.DEFAULT_GOAL := build
