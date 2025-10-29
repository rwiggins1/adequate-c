# Configuration
BUILD_DIR := build
EXECUTABLE := adequatec
SRC_DIR := src

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

# Show help
help:
	@echo "Adequate-C Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make build   - Build the project"
	@echo "  make clean   - Remove $(BUILD_DIR) directory"
	@echo "  make format  - Format all source code"
	@echo "  make run     - Build and run $(EXECUTABLE)"
	@echo "  make help    - Show this help message"

.DEFAULT_GOAL := build
