#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/type.hpp"
#include "gtest/gtest.h"
#include <memory>
#include <string>

using namespace frontend;
using namespace frontend::ast;
using namespace frontend::types;

namespace {
std::unique_ptr<DeclAST> parseVarDecl(const std::string &src,
				      ErrorReporter &errors) {
	Lexer lexer(src, errors);
	Parser parser(lexer, errors);
	return parser.parseVarDecl();
}

// Parse a declaration expected to succeed and return it downcast.
std::unique_ptr<VariableDeclarationAST> expectVarDecl(const std::string &src,
						      ErrorReporter &errors) {
	auto decl = parseVarDecl(src, errors);
	// parseVarDecl only ever produces VariableDeclarationAST or null,
	// so the failed-cast leak case cannot occur.
	return std::unique_ptr<VariableDeclarationAST>(
	    dynamic_cast<VariableDeclarationAST *>(decl.release()));
}

// Parse a declaration expected to fail: null result plus an error.
void expectParseError(const std::string &src) {
	ErrorReporter errors;
	auto decl = parseVarDecl(src, errors);
	EXPECT_EQ(decl, nullptr) << src;
	EXPECT_TRUE(errors.hasErrors()) << src;
}
} // namespace

TEST(ParserVarDecl, PlainDeclaration) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var int x;", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	EXPECT_EQ(decl->getName(), "x");
	ASSERT_NE(decl->getType(), nullptr);
	EXPECT_EQ(decl->getType()->toString(), "int");
	EXPECT_FALSE(decl->hasInit());
	EXPECT_FALSE(decl->isArray());
}

TEST(ParserVarDecl, LiteralInitializer) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var int x = 42;", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	EXPECT_TRUE(decl->hasInit());
	EXPECT_FALSE(decl->isArray());

	auto *literal = dynamic_cast<NumberLiteralAST *>(decl->getInit());
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(literal->getValue(), 42.0);
}

TEST(ParserVarDecl, ExpressionInitializer) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var int x = length + 1;", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	ASSERT_TRUE(decl->hasInit());
	EXPECT_FALSE(decl->isArray());

	auto *binary = dynamic_cast<BinaryExprAST *>(decl->getInit());
	ASSERT_NE(binary, nullptr);
	EXPECT_EQ(binary->getOperator(), BinaryOp::ADD);
}

TEST(ParserVarDecl, ArrayLiteralSize) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var int arr[10];", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	EXPECT_EQ(decl->getName(), "arr");
	EXPECT_FALSE(decl->hasInit());
	ASSERT_TRUE(decl->isArray());

	auto *literal = dynamic_cast<NumberLiteralAST *>(decl->getArraySize());
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(literal->getValue(), 10.0);
}

TEST(ParserVarDecl, ArrayExpressionSize) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var int arr[length + 1];", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	ASSERT_TRUE(decl->isArray());
	EXPECT_FALSE(decl->hasInit());

	auto *binary = dynamic_cast<BinaryExprAST *>(decl->getArraySize());
	ASSERT_NE(binary, nullptr);
	EXPECT_EQ(binary->getOperator(), BinaryOp::ADD);
}

TEST(ParserVarDecl, ConstQualifiedType) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var const int x = 1;", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());

	auto *const_type = dynamic_cast<ConstType *>(decl->getType());
	ASSERT_NE(const_type, nullptr);
	EXPECT_EQ(const_type->getInnerType()->toString(), "int");
}

TEST(ParserVarDecl, MissingVarKeyword) { expectParseError("int x;"); }

TEST(ParserVarDecl, MissingIdentifier) { expectParseError("var int ;"); }

TEST(ParserVarDecl, MissingSemicolonAfterInitializer) {
	expectParseError("var int x = 5 var");
}

TEST(ParserVarDecl, BadTokenAfterName) { expectParseError("var int x 5;"); }

TEST(ParserVarDecl, MissingClosingBracket) {
	expectParseError("var int arr[5;");
}

TEST(ParserVarDecl, MissingSemicolonAfterArray) {
	expectParseError("var int arr[5] var");
}
