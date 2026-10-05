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

# AddressSanitizer / UndefinedBehaviorSanitizer instrumentation
option(ADQ_ENABLE_ASAN "Build source and test targets with AddressSanitizer" OFF)
option(ADQ_ENABLE_UBSAN "Build source and test targets with UndefinedBehaviorSanitizer" OFF)

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
    src/ast/printer.cpp
    src/parser/parser.cpp
)

# Build executable named 'adequatec'; the binary on disk is 'adq'
add_executable(adequatec ${SOURCES})
set_target_properties(adequatec PROPERTIES OUTPUT_NAME adq)

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
    tests/unit/test_parser_decl.cpp
    tests/unit/test_types.cpp

    src/ast/expr.cpp
    src/ast/stmt.cpp
    src/ast/decl.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
)
target_include_directories(ast_gtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ast_gtest GTest::gtest_main)

# Google Test for lexer -> parser integration
add_executable(integration_gtest
    tests/integration/test_lexer_parser.cpp

    src/ast/expr.cpp
    src/ast/stmt.cpp
    src/ast/decl.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
)
target_include_directories(integration_gtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(integration_gtest GTest::gtest_main)

# Register test
include(GoogleTest)
gtest_discover_tests(ast_gtest)
gtest_discover_tests(integration_gtest)

# All buildable targets that source/test instrumentation (coverage, sanitizers) applies to
set(ADQ_ALL_TARGETS adequatec test_lexer test_ast ast_gtest integration_gtest)

# =======================================================================
# Sanitizers (ASan/UBSan)
# =======================================================================
if(ADQ_ENABLE_ASAN OR ADQ_ENABLE_UBSAN)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING "ADQ_ENABLE_ASAN/ADQ_ENABLE_UBSAN require GCC or Clang; ${CMAKE_CXX_COMPILER_ID} is unsupported")
    endif()

    set(SANITIZE_FLAGS "")
    if(ADQ_ENABLE_ASAN)
        list(APPEND SANITIZE_FLAGS -fsanitize=address -fno-omit-frame-pointer)
    endif()
    if(ADQ_ENABLE_UBSAN)
        # -fno-sanitize-recover makes UB fatal (abort) instead of just printing
        # a runtime error and continuing, which would let a test exit 0 despite UB
        list(APPEND SANITIZE_FLAGS -fsanitize=undefined -fno-sanitize-recover=undefined)
    endif()

    foreach(san_target ${ADQ_ALL_TARGETS})
        target_compile_options(${san_target} PRIVATE ${SANITIZE_FLAGS} -g)
        target_link_options(${san_target} PRIVATE ${SANITIZE_FLAGS})
    endforeach()
endif()

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

    foreach(cov_target ${ADQ_ALL_TARGETS})
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
            DEPENDS ${ADQ_ALL_TARGETS}
            COMMENT "Running tests and generating gcovr coverage report"
            VERBATIM
        )
    endif()
endif()

# print useful info
message(STATUS "Building Adequate-C compiler")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
