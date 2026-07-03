cmake_minimum_required(VERSION 3.13)

# Project name
project(Adequate-C CXX)

# Generate compile_commands.json for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# gcov/gcovr coverage instrumentation
option(ADQ_ENABLE_COVERAGE "Instrument source and test targets with --coverage (gcov/gcovr)" OFF)

# Warning flags for g++
add_compile_options(-Wall -Wextra -pedantic)

# Debug symbols
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Source files
set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/ast/expr.cpp
    src/ast/stmt.cpp
    src/ast/decl.cpp
    src/parser/parser.cpp
)

# Build executable named 'adequatec'
add_executable(adequatec ${SOURCES})

# Allow #include "frontend/lexer/lexer.hpp" instead of #include "src/frontend/lexer/lexer.hpp"
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
    src/lexer/lexer.cpp
)
target_include_directories(test_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Manual Test executable for ast
add_executable(test_ast
    tests/manual/test_ast.cpp
    src/lexer/lexer.cpp
    src/ast/expr.cpp
    src/ast/stmt.cpp
    src/ast/decl.cpp
    src/parser/parser.cpp
)
target_include_directories(test_ast PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Google Test for AST and Lexer
add_executable(ast_gtest
    tests/unit/test_token.cpp
    tests/unit/test_ast.cpp
    tests/unit/test_lexer.cpp
    tests/unit/test_parser_expr.cpp
    tests/unit/test_types.cpp

    src/ast/expr.cpp
    src/ast/stmt.cpp
    src/ast/decl.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
)
target_include_directories(ast_gtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ast_gtest GTest::gtest_main)

# Register test
include(GoogleTest)
gtest_discover_tests(ast_gtest)

# =======================================================================
# Coverage (gcov/gcovr)
# =======================================================================
if(ADQ_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING "ADQ_ENABLE_COVERAGE requires GCC or Clang; ${CMAKE_CXX_COMPILER_ID} is unsupported")
    endif()
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "ADQ_ENABLE_COVERAGE is on but CMAKE_BUILD_TYPE is not Debug; "
                         "optimizations can make gcov line/branch data unreliable")
    endif()

    set(COVERAGE_TARGETS adequatec test_lexer test_ast ast_gtest)
    foreach(cov_target ${COVERAGE_TARGETS})
        target_compile_options(${cov_target} PRIVATE --coverage -O0 -g)
        target_link_options(${cov_target} PRIVATE --coverage)
    endforeach()

    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        message(WARNING "gcovr not found; 'coverage' target will not be available")
    else()
        set(COVERAGE_HTML_DIR ${CMAKE_BINARY_DIR}/coverage)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_HTML_DIR}
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${GCOVR_EXECUTABLE}
                    --root ${CMAKE_SOURCE_DIR}
                    --filter ${CMAKE_SOURCE_DIR}/src/
                    --exclude-unreachable-branches
                    --print-summary
                    --html-details ${COVERAGE_HTML_DIR}/index.html
                    ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS ${COVERAGE_TARGETS}
            COMMENT "Running tests and generating gcovr coverage report"
            VERBATIM
        )
    endif()
endif()

# print useful info
message(STATUS "Building Adequate-C compiler")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
