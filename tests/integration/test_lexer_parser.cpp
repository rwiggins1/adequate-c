// Integration tests: run real source text through the full
// Lexer -> Parser pipeline and validate the resulting AST shape
// and the diagnostics produced along the way.
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <memory>
#include <string>

using namespace frontend;
using namespace frontend::ast;

namespace {
std::unique_ptr<ExprAST> parsePipeline(const std::string &src,
				       ErrorReporter &errors) {
	Lexer lexer(src, errors);
	Parser parser(lexer, errors);
	return parser.parseExpression();
}
} // namespace

// result + compute(x, y) * 2  =>  result + (compute(x, y) * 2)
TEST(LexerParserIntegration, CallInsideArithmetic) {
	ErrorReporter errors;
	auto expr = parsePipeline("result + compute(x, y) * 2;", errors);

	auto *add = dynamic_cast<BinaryExprAST *>(expr.get());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getOperator(), BinaryOp::ADD);

	auto *lhs = dynamic_cast<VariableExprAST *>(add->getLhs());
	ASSERT_NE(lhs, nullptr);
	EXPECT_EQ(lhs->getName(), "result");

	auto *mul = dynamic_cast<BinaryExprAST *>(add->getRhs());
	ASSERT_NE(mul, nullptr);
	EXPECT_EQ(mul->getOperator(), BinaryOp::MUL);

	auto *call = dynamic_cast<CallExprAST *>(mul->getLhs());
	ASSERT_NE(call, nullptr);
	auto *callee = dynamic_cast<VariableExprAST *>(call->getCallee());
	ASSERT_NE(callee, nullptr);
	EXPECT_EQ(callee->getName(), "compute");
	ASSERT_EQ(call->getArgs().size(), 2);

	auto *two = dynamic_cast<NumberLiteralAST *>(mul->getRhs());
	ASSERT_NE(two, nullptr);
	EXPECT_DOUBLE_EQ(two->getValue(), 2.0);

	EXPECT_FALSE(errors.hasErrors());
}

// (a + b) * (c - d)
TEST(LexerParserIntegration, ParenthesizedGroups) {
	ErrorReporter errors;
	auto expr = parsePipeline("(a + b) * (c - d);", errors);

	auto *mul = dynamic_cast<BinaryExprAST *>(expr.get());
	ASSERT_NE(mul, nullptr);
	EXPECT_EQ(mul->getOperator(), BinaryOp::MUL);

	auto *add = dynamic_cast<BinaryExprAST *>(mul->getLhs());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getOperator(), BinaryOp::ADD);

	auto *sub = dynamic_cast<BinaryExprAST *>(mul->getRhs());
	ASSERT_NE(sub, nullptr);
	EXPECT_EQ(sub->getOperator(), BinaryOp::SUB);

	EXPECT_FALSE(errors.hasErrors());
}

// Comments and newlines between tokens must be invisible to the parser
TEST(LexerParserIntegration, CommentsAndWhitespaceAreSkipped) {
	ErrorReporter errors;
	auto expr =
	    parsePipeline("1 /* mid */ + // to end of line\n\t 2;", errors);

	auto *add = dynamic_cast<BinaryExprAST *>(expr.get());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getOperator(), BinaryOp::ADD);
	EXPECT_FALSE(errors.hasErrors());
}

// Redundant parentheses collapse to the inner expression
TEST(LexerParserIntegration, DeeplyNestedParens) {
	ErrorReporter errors;
	auto expr = parsePipeline("((((42))));", errors);

	auto *literal = dynamic_cast<NumberLiteralAST *>(expr.get());
	ASSERT_NE(literal, nullptr);
	EXPECT_DOUBLE_EQ(literal->getValue(), 42.0);
	EXPECT_FALSE(errors.hasErrors());
}

// a < b == c < d  =>  (a < b) == (c < d)
TEST(LexerParserIntegration, RelationalBindsTighterThanEquality) {
	ErrorReporter errors;
	auto expr = parsePipeline("a < b == c < d;", errors);

	auto *eq = dynamic_cast<BinaryExprAST *>(expr.get());
	ASSERT_NE(eq, nullptr);
	EXPECT_EQ(eq->getOperator(), BinaryOp::EQ);

	auto *lhs = dynamic_cast<BinaryExprAST *>(eq->getLhs());
	ASSERT_NE(lhs, nullptr);
	EXPECT_EQ(lhs->getOperator(), BinaryOp::LT);

	auto *rhs = dynamic_cast<BinaryExprAST *>(eq->getRhs());
	ASSERT_NE(rhs, nullptr);
	EXPECT_EQ(rhs->getOperator(), BinaryOp::LT);

	EXPECT_FALSE(errors.hasErrors());
}

// -a * !b  =>  (-a) * (!b)
TEST(LexerParserIntegration, UnaryInsideBinary) {
	ErrorReporter errors;
	auto expr = parsePipeline("-a * !b;", errors);

	auto *mul = dynamic_cast<BinaryExprAST *>(expr.get());
	ASSERT_NE(mul, nullptr);
	EXPECT_EQ(mul->getOperator(), BinaryOp::MUL);

	auto *neg = dynamic_cast<UnaryExprAST *>(mul->getLhs());
	ASSERT_NE(neg, nullptr);
	EXPECT_EQ(neg->getOperator(), UnaryOp::MINUS);

	auto *not_expr = dynamic_cast<UnaryExprAST *>(mul->getRhs());
	ASSERT_NE(not_expr, nullptr);
	EXPECT_EQ(not_expr->getOperator(), UnaryOp::NOT);

	EXPECT_FALSE(errors.hasErrors());
}

// A bad character produces a lexer diagnostic and the parser then
// reports its own error for the INVALID token; both phases must land
// in the same ErrorReporter.
TEST(LexerParserIntegration, ErrorsFromBothPhasesAccumulate) {
	ErrorReporter errors;
	auto expr = parsePipeline("1 + $;", errors);

	EXPECT_EQ(expr, nullptr);
	ASSERT_TRUE(errors.hasErrors());
	EXPECT_GE(errors.getErrorCount(), 2);

	const auto &all = errors.getErrors();
	EXPECT_TRUE(std::ranges::any_of(all, [](const CompilerError &err) {
		return err.phase == ErrorPhase::LEXER;
	}));
	EXPECT_TRUE(std::ranges::any_of(all, [](const CompilerError &err) {
		return err.phase == ErrorPhase::PARSER;
	}));
}

// Literals of every kind survive the whole pipeline inside one call
TEST(LexerParserIntegration, MixedLiteralArguments) {
	ErrorReporter errors;
	auto expr =
	    parsePipeline("log(42, 3.14, 'c', \"msg\", true, false);", errors);

	auto *call = dynamic_cast<CallExprAST *>(expr.get());
	ASSERT_NE(call, nullptr);

	const auto &args = call->getArgs();
	ASSERT_EQ(args.size(), 6);
	EXPECT_NE(dynamic_cast<NumberLiteralAST *>(args[0].get()), nullptr);
	EXPECT_NE(dynamic_cast<NumberLiteralAST *>(args[1].get()), nullptr);
	EXPECT_NE(dynamic_cast<CharLiteralAST *>(args[2].get()), nullptr);
	EXPECT_NE(dynamic_cast<StringLiteralAST *>(args[3].get()), nullptr);
	EXPECT_NE(dynamic_cast<BoolLiteralAST *>(args[4].get()), nullptr);
	EXPECT_NE(dynamic_cast<BoolLiteralAST *>(args[5].get()), nullptr);
	EXPECT_FALSE(errors.hasErrors());
}
