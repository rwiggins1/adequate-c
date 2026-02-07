cmake_minimum_required(VERSION 3.10)

# Project name
project(Adequate-C CXX)

# Generate compile_commands.json for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warning flags for g++
add_compile_options(-Wall -Wextra -pedantic)

# Debug symbols
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Source files
set(SOURCES
    src/main.cpp
    src/frontend/lexer.cpp
    src/frontend/ast.cpp
    src/frontend/parser.cpp
)

# Build executable named 'adequatec'
add_executable(adequatec ${SOURCES})

# Allow #include "frontend/lexer.hpp" instead of #include "src/frontend/lexer.hpp"
target_include_directories(adequatec PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Google Test setup
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# =======================================================================
# Testing
# =======================================================================

# Manual Test executable for lexer
add_executable(test_lexer
    tests/manual/test_lexer.cpp
    src/frontend/lexer.cpp
)
target_include_directories(test_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Manual Test executable for ast
add_executable(test_ast
    tests/manual/test_ast.cpp
    src/frontend/lexer.cpp
    src/frontend/ast.cpp
    src/frontend/parser.cpp
)
target_include_directories(test_ast PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Google Test for AST and Lexer
add_executable(ast_gtest
    tests/unit/test_ast.cpp
    tests/unit/test_lexer.cpp
    src/frontend/ast.cpp
    src/frontend/lexer.cpp
    src/frontend/parser.cpp
)
target_include_directories(ast_gtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ast_gtest GTest::gtest_main)

# Register test
include(GoogleTest)
gtest_discover_tests(ast_gtest)

# print useful info
message(STATUS "Building Adequate-C compiler")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
