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
    src/frontend/parser.cpp
)

# Build executable named 'adequatec'
add_executable(adequatec ${SOURCES})

# Allow #include "frontend/lexer.hpp" instead of #include "src/frontend/lexer.hpp"
target_include_directories(adequatec PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test executable for lexer
add_executable(test_lexer
    tests/test_lexer.cpp
    src/frontend/lexer.cpp
)
target_include_directories(test_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# print useful info
message(STATUS "Building Adequate-C compiler")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
