#include "../test_helpers.hpp"
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
	    expectNode<VariableDeclarationAST>(decl.release()));
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

	auto *literal = expectNode<NumberLiteralAST>(decl->getInit());
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

	auto *binary = expectNode<BinaryExprAST>(decl->getInit());
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

	auto *literal = expectNode<NumberLiteralAST>(decl->getArraySize());
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

	auto *binary = expectNode<BinaryExprAST>(decl->getArraySize());
	ASSERT_NE(binary, nullptr);
	EXPECT_EQ(binary->getOperator(), BinaryOp::ADD);
}

TEST(ParserVarDecl, ConstQualifiedType) {
	ErrorReporter errors;
	auto decl = expectVarDecl("var const int x = 1;", errors);

	ASSERT_NE(decl, nullptr);
	EXPECT_FALSE(errors.hasErrors());

	auto *const_type = expectNode<ConstType>(decl->getType());
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

TEST(ParserProto, UnqualifiedName) {
	ErrorReporter errors;
	Lexer lexer("func add(int x, int y) -> int", errors);
	Parser parser(lexer, errors);

	auto proto = parser.parseProto();
	ASSERT_NE(proto, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	EXPECT_EQ(proto->getName(), "add");
	EXPECT_FALSE(proto->getQualifiedName().isQualified());
}

TEST(ParserProto, QualifiedName) {
	ErrorReporter errors;
	Lexer lexer("func math::vec::dot(int x, int y) -> int", errors);
	Parser parser(lexer, errors);

	auto proto = parser.parseProto();
	ASSERT_NE(proto, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	EXPECT_EQ(proto->getName(), "dot");

	const auto &qname = proto->getQualifiedName();
	ASSERT_EQ(qname.qualifiers.size(), 2);
	EXPECT_EQ(qname.qualifiers[0], "math");
	EXPECT_EQ(qname.qualifiers[1], "vec");
	EXPECT_EQ(qname.str(), "math::vec::dot");
}

TEST(ParserProto, QualifiedNameMissingIdentifier) {
	ErrorReporter errors;
	Lexer lexer("func math::(int x) -> int", errors);
	Parser parser(lexer, errors);

	auto proto = parser.parseProto();
	EXPECT_EQ(proto, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

namespace {
std::unique_ptr<ProgramAST> parseProgram(const std::string &src,
					 ErrorReporter &errors) {
	Lexer lexer(src, errors);
	Parser parser(lexer, errors);
	return parser.parseProgram();
}
} // namespace

TEST(ParserNamespace, HoldsDeclarations) {
	ErrorReporter errors;
	auto program = parseProgram("namespace math {"
				    "  func add(int x, int y) -> int { return x; }"
				    "  func sub(int x, int y) -> int { return x; }"
				    "}",
				    errors);

	ASSERT_NE(program, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	ASSERT_EQ(program->getDeclarations().size(), 1);

	auto *ns = expectNode<NamespaceAST>(program->getDeclarations()[0].get());
	ASSERT_NE(ns, nullptr);
	EXPECT_EQ(ns->getName(), "math");
	ASSERT_EQ(ns->getDeclarations().size(), 2);

	auto *add = expectNode<FunctionAST>(ns->getDeclarations()[0].get());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getProto()->getName(), "add");
}

TEST(ParserNamespace, Nested) {
	ErrorReporter errors;
	auto program = parseProgram("namespace outer { namespace inner {} }", errors);

	ASSERT_NE(program, nullptr);
	EXPECT_FALSE(errors.hasErrors());

	auto *outer =
	    expectNode<NamespaceAST>(program->getDeclarations()[0].get());
	ASSERT_NE(outer, nullptr);
	ASSERT_EQ(outer->getDeclarations().size(), 1);

	auto *inner =
	    expectNode<NamespaceAST>(outer->getDeclarations()[0].get());
	ASSERT_NE(inner, nullptr);
	EXPECT_EQ(inner->getName(), "inner");
	EXPECT_TRUE(inner->getDeclarations().empty());
}

TEST(ParserNamespace, MissingClosingBrace) {
	ErrorReporter errors;
	auto program = parseProgram("namespace math {", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

TEST(ParserProgram, StrayClosingBrace) {
	ErrorReporter errors;
	auto program = parseProgram("}", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

TEST(ParserStruct, FieldsAndMethods) {
	ErrorReporter errors;
	auto program = parseProgram("struct Point {"
				    "  var int x;"
				    "  var int y;"
				    "  func length() -> int { return x; }"
				    "}",
				    errors);

	ASSERT_NE(program, nullptr);
	EXPECT_FALSE(errors.hasErrors());
	ASSERT_EQ(program->getDeclarations().size(), 1);

	auto *st = expectNode<StructAST>(program->getDeclarations()[0].get());
	ASSERT_NE(st, nullptr);
	EXPECT_EQ(st->getName(), "Point");
	ASSERT_EQ(st->getFields().size(), 2);
	EXPECT_EQ(st->getFields()[0]->getName(), "x");
	EXPECT_EQ(st->getFields()[1]->getName(), "y");
	ASSERT_EQ(st->getMethods().size(), 1);
	EXPECT_EQ(st->getMethods()[0]->getProto()->getName(), "length");
}

TEST(ParserStruct, Empty) {
	ErrorReporter errors;
	auto program = parseProgram("struct Empty {}", errors);

	ASSERT_NE(program, nullptr);
	EXPECT_FALSE(errors.hasErrors());

	auto *st = expectNode<StructAST>(program->getDeclarations()[0].get());
	ASSERT_NE(st, nullptr);
	EXPECT_TRUE(st->getFields().empty());
	EXPECT_TRUE(st->getMethods().empty());
}

TEST(ParserStruct, MissingName) {
	ErrorReporter errors;
	auto program = parseProgram("struct {}", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

TEST(ParserStruct, MissingOpeningBrace) {
	ErrorReporter errors;
	auto program = parseProgram("struct Point var int x; }", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

TEST(ParserStruct, MissingClosingBrace) {
	ErrorReporter errors;
	auto program = parseProgram("struct Point { var int x;", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}

TEST(ParserStruct, InvalidFieldPropagatesFailure) {
	ErrorReporter errors;
	auto program = parseProgram("struct Point { var int; }", errors);

	EXPECT_EQ(program, nullptr);
	EXPECT_TRUE(errors.hasErrors());
}
