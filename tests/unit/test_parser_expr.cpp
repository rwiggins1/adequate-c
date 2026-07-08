#include "../test_helpers.hpp"
#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/type.hpp"
#include "gtest/gtest.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace frontend;
using namespace frontend::ast;
using namespace frontend::types;

namespace {
// Note: expressions need a terminator like ';' after them because the
// postfix follow-set does not include EOF.
std::unique_ptr<ExprAST> parseExpr(const std::string &src,
				   ErrorReporter &errors) {
	Lexer lexer(src, errors);
	Parser parser(lexer, errors);
	return parser.parseExpression();
}

std::unique_ptr<types::Type> parseType(const std::string &src,
				       ErrorReporter &errors) {
	Lexer lexer(src, errors);
	Parser parser(lexer, errors);
	return parser.parseType();
}

// Parse "<lhs> <op> <rhs>;" and check the resulting binary node
void expectBinaryOp(const std::string &src, BinaryOp expected_op) {
	ErrorReporter errors;
	auto expr = parseExpr(src, errors);

	auto *binary = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(binary, nullptr) << src;
	EXPECT_EQ(binary->getOperator(), expected_op) << src;
	EXPECT_NE(binary->getLhs(), nullptr) << src;
	EXPECT_NE(binary->getRhs(), nullptr) << src;
	EXPECT_FALSE(errors.hasErrors()) << src;
}

// Parse "<op><operand>;" and check the resulting unary node
void expectUnaryOp(const std::string &src, UnaryOp expected_op) {
	ErrorReporter errors;
	auto expr = parseExpr(src, errors);

	auto *unary = expectNode<UnaryExprAST>(expr.get());
	ASSERT_NE(unary, nullptr) << src;
	EXPECT_EQ(unary->getOperator(), expected_op) << src;
	EXPECT_NE(unary->getOperand(), nullptr) << src;
	EXPECT_FALSE(errors.hasErrors()) << src;
}
} // namespace

TEST(ParserExpr, ParseLiteral) {
	{
		ErrorReporter errors;
		Lexer lexer("2", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast = parser.parseLiteral();

		auto literal_ast =
		    expectNode<ast::NumberLiteralAST>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		double val = literal_ast->getValue();
		ASSERT_EQ(val, 2.0);
	}
	{
		ErrorReporter errors;
		Lexer lexer("'a'", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast = parser.parseLiteral();

		auto literal_ast =
		    expectNode<ast::CharLiteralAST>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		char val = literal_ast->getValue();
		ASSERT_EQ(val, 'a');
	}
	{
		ErrorReporter errors;
		Lexer lexer("\"Hello, World!\"", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast = parser.parseLiteral();

		auto literal_ast =
		    expectNode<ast::StringLiteralAST>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		std::string val = literal_ast->getValue();
		ASSERT_EQ(val, "\"Hello, World!\"");
	}
	{
		ErrorReporter errors;
		Lexer lexer("true", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast = parser.parseLiteral();

		auto literal_ast =
		    expectNode<ast::BoolLiteralAST>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, true);
	}
	{
		ErrorReporter errors;
		Lexer lexer("false", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast = parser.parseLiteral();

		auto literal_ast =
		    expectNode<ast::BoolLiteralAST>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, false);
	}
}

TEST(ParserExpr, ParserPrimitiveType) {
	std::vector<std::string> primitives = {
	    "int", "float", "double", "bool", "char", "string", "void"};
	for (const auto &name : primitives) {
		ErrorReporter errors;
		Lexer lexer(name, errors);
		Parser parser(lexer, errors);

		auto prim_type = parser.parsePrimitiveType();
		ASSERT_NE(prim_type, nullptr) << name;
		EXPECT_EQ(prim_type->toString(), name);
		EXPECT_FALSE(errors.hasErrors()) << name;
	}
}

TEST(ParserExpr, ParserPrimitiveTypeError) {
	ErrorReporter errors;
	Lexer lexer("foo", errors);
	Parser parser(lexer, errors);

	auto prim_type = parser.parsePrimitiveType();
	ASSERT_EQ(prim_type, nullptr);
	ASSERT_TRUE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeConst) {
	ErrorReporter errors;
	auto type = parseType("const int", errors);

	auto *const_type = expectNode<types::ConstType>(type.get());
	ASSERT_NE(const_type, nullptr);
	EXPECT_EQ(const_type->toString(), "const int");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeStatic) {
	ErrorReporter errors;
	auto type = parseType("static bool", errors);

	auto *static_type = expectNode<types::StaticType>(type.get());
	ASSERT_NE(static_type, nullptr);
	EXPECT_EQ(static_type->toString(), "static bool");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeStaticConst) {
	ErrorReporter errors;
	auto type = parseType("static const double", errors);

	ASSERT_NE(type, nullptr);
	EXPECT_EQ(type->toString(), "static const double");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeStruct) {
	ErrorReporter errors;
	auto type = parseType("struct Point", errors);

	auto *struct_type = expectNode<types::StructType>(type.get());
	ASSERT_NE(struct_type, nullptr);
	EXPECT_EQ(struct_type->getName(), "Point");
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeStructMissingName) {
	ErrorReporter errors;
	auto type = parseType("struct ;", errors);

	ASSERT_EQ(type, nullptr);
	ASSERT_TRUE(errors.hasErrors());
}

TEST(ParserExpr, ParseTypeConstMissingInner) {
	ErrorReporter errors;
	auto type = parseType("const ;", errors);

	ASSERT_EQ(type, nullptr);
	ASSERT_TRUE(errors.hasErrors());
}

TEST(ParserExpr, UnaryOperatorPredicate) {
	std::vector<std::string> unary_ops = {"&", "*", "+", "-", "~", "!"};
	for (const auto &op : unary_ops) {
		ErrorReporter errors;
		Lexer lexer(op, errors);
		Parser parser(lexer, errors);
		EXPECT_TRUE(parser.unaryOperator()) << op;
	}

	std::vector<std::string> not_unary = {"x", "1", "/", "=="};
	for (const auto &op : not_unary) {
		ErrorReporter errors;
		Lexer lexer(op, errors);
		Parser parser(lexer, errors);
		EXPECT_FALSE(parser.unaryOperator()) << op;
	}
}

TEST(ParserExpr, AssignmentOperatorPredicate) {
	std::vector<std::string> assign_ops = {
	    "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|="};
	for (const auto &op : assign_ops) {
		ErrorReporter errors;
		Lexer lexer(op, errors);
		Parser parser(lexer, errors);
		EXPECT_TRUE(parser.assignmentOperator()) << op;
	}

	std::vector<std::string> not_assign = {"+", "<", "x"};
	for (const auto &op : not_assign) {
		ErrorReporter errors;
		Lexer lexer(op, errors);
		Parser parser(lexer, errors);
		EXPECT_FALSE(parser.assignmentOperator()) << op;
	}
}

TEST(ParserExpr, ParserPrimaryExpr) {
	{
		ErrorReporter errors;
		Lexer lexer("2", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto prim_expr_ast = parser.parsePrimaryExpr();

		auto prim_expr =
		    expectNode<ast::NumberLiteralAST>(prim_expr_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
	{
		ErrorReporter errors;
		Lexer lexer("some_variable", errors);
		Parser parser(lexer, errors);

		auto prim_expr_ast = parser.parsePrimaryExpr();

		auto *var_expr =
		    expectNode<ast::VariableExprAST>(prim_expr_ast.get());
		ASSERT_NE(var_expr, nullptr);
		EXPECT_EQ(var_expr->getName(), "some_variable");
	}
	{
		// parenthesized expression
		ErrorReporter errors;
		Lexer lexer("(1 + 2)", errors);
		Parser parser(lexer, errors);

		auto prim_expr_ast = parser.parsePrimaryExpr();

		auto *binary =
		    expectNode<ast::BinaryExprAST>(prim_expr_ast.get());
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), BinaryOp::ADD);
		EXPECT_FALSE(errors.hasErrors());
	}
}

TEST(ParserExpr, ParserPrimaryExprErrors) {
	{
		// unclosed parenthesis
		ErrorReporter errors;
		Lexer lexer("(1 + 2;", errors);
		Parser parser(lexer, errors);

		auto prim_expr_ast = parser.parsePrimaryExpr();
		ASSERT_EQ(prim_expr_ast, nullptr);
		ASSERT_TRUE(errors.hasErrors());
	}
	{
		// no identifier, literal or '('
		ErrorReporter errors;
		Lexer lexer(";", errors);
		Parser parser(lexer, errors);

		auto prim_expr_ast = parser.parsePrimaryExpr();
		ASSERT_EQ(prim_expr_ast, nullptr);
		ASSERT_TRUE(errors.hasErrors());
	}
}

TEST(ParserExpr, ParserPostfixExpr) {
	{
		ErrorReporter errors;
		Lexer lexer("2;", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto postfix_ast = parser.parsePostfixExpr();

		auto prim_expr =
		    expectNode<ast::NumberLiteralAST>(postfix_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
}

TEST(ParserExpr, PostfixCall) {
	ErrorReporter errors;
	auto expr = parseExpr("add(5, 3);", errors);

	auto *call = expectNode<CallExprAST>(expr.get());
	ASSERT_NE(call, nullptr);

	auto *callee = expectNode<VariableExprAST>(call->getCallee());
	ASSERT_NE(callee, nullptr);
	EXPECT_EQ(callee->getName(), "add");

	const auto &args = call->getArgs();
	ASSERT_EQ(args.size(), 2);
	auto *arg0 = expectNode<NumberLiteralAST>(args[0].get());
	auto *arg1 = expectNode<NumberLiteralAST>(args[1].get());
	ASSERT_NE(arg0, nullptr);
	ASSERT_NE(arg1, nullptr);
	EXPECT_DOUBLE_EQ(arg0->getValue(), 5.0);
	EXPECT_DOUBLE_EQ(arg1->getValue(), 3.0);
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, PostfixCallWithExprArgs) {
	ErrorReporter errors;
	auto expr = parseExpr("f(1 + 2, g(3));", errors);

	auto *call = expectNode<CallExprAST>(expr.get());
	ASSERT_NE(call, nullptr);

	const auto &args = call->getArgs();
	ASSERT_EQ(args.size(), 2);
	EXPECT_NE(expectNode<BinaryExprAST>(args[0].get()), nullptr);
	EXPECT_NE(expectNode<CallExprAST>(args[1].get()), nullptr);
	EXPECT_FALSE(errors.hasErrors());
}

TEST(ParserExpr, PostfixIncrementDecrement) {
	{
		ErrorReporter errors;
		auto expr = parseExpr("x++;", errors);

		auto *unary = expectNode<UnaryExprAST>(expr.get());
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), UnaryOp::POST_INCREMENT);

		auto *operand =
		    expectNode<VariableExprAST>(unary->getOperand());
		ASSERT_NE(operand, nullptr);
		EXPECT_EQ(operand->getName(), "x");
		EXPECT_FALSE(errors.hasErrors());
	}
	{
		ErrorReporter errors;
		auto expr = parseExpr("y--;", errors);

		auto *unary = expectNode<UnaryExprAST>(expr.get());
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), UnaryOp::POST_DECREMENT);
		EXPECT_FALSE(errors.hasErrors());
	}
}

TEST(ParserExpr, PostfixSubscriptAndMemberAccess) {
	{
		ErrorReporter errors;
		auto expr = parseExpr("a[1];", errors);
		EXPECT_NE(expr, nullptr);
		EXPECT_FALSE(errors.hasErrors());
	}
	{
		ErrorReporter errors;
		auto expr = parseExpr("p.x;", errors);
		EXPECT_NE(expr, nullptr);
		EXPECT_FALSE(errors.hasErrors());
	}
	{
		ErrorReporter errors;
		auto expr = parseExpr("math::add;", errors);
		EXPECT_NE(expr, nullptr);
		EXPECT_FALSE(errors.hasErrors());
	}
}

TEST(ParserExpr, UnaryExpr) {
	expectUnaryOp("-2;", UnaryOp::MINUS);
	expectUnaryOp("+2;", UnaryOp::PLUS);
	expectUnaryOp("!true;", UnaryOp::NOT);
	expectUnaryOp("~1;", UnaryOp::BIT_NOT);
	expectUnaryOp("*ptr;", UnaryOp::MUL);
}

TEST(ParserExpr, ParserMultiplicative) {
	{
		ErrorReporter errors;
		Lexer lexer("2 * 2;", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<types::Type>
		auto mult_ast = parser.parseMultiplicativeExpr();

		auto mult_expr = expectNode<ast::BinaryExprAST>(mult_ast.get());
		ASSERT_NE(mult_expr, nullptr);

		ast::ExprAST *lhs_expr = mult_expr->getLhs();
		auto lhs = expectNode<ast::NumberLiteralAST>(lhs_expr);
		ASSERT_NE(lhs, nullptr);

		BinaryOp op = mult_expr->getOperator();
		ASSERT_EQ(op, BinaryOp::MUL);

		ast::ExprAST *rhs_expr = mult_expr->getRhs();
		auto rhs = expectNode<ast::NumberLiteralAST>(rhs_expr);
		ASSERT_NE(rhs, nullptr);
	}
}

TEST(ParserExpr, BinaryOperators) {
	expectBinaryOp("6 / 2;", BinaryOp::DIV);
	expectBinaryOp("7 % 3;", BinaryOp::MOD);
	expectBinaryOp("1 + 2;", BinaryOp::ADD);
	expectBinaryOp("5 - 3;", BinaryOp::SUB);
	expectBinaryOp("1 << 2;", BinaryOp::SHL);
	expectBinaryOp("8 >> 1;", BinaryOp::SHR);
	expectBinaryOp("1 < 2;", BinaryOp::LT);
	expectBinaryOp("2 > 1;", BinaryOp::GT);
	expectBinaryOp("1 <= 2;", BinaryOp::LE);
	expectBinaryOp("2 >= 1;", BinaryOp::GE);
	expectBinaryOp("1 == 1;", BinaryOp::EQ);
	expectBinaryOp("1 != 2;", BinaryOp::NEQ);
	expectBinaryOp("1 & 3;", BinaryOp::BIT_AND);
	expectBinaryOp("1 ^ 3;", BinaryOp::BIT_XOR);
	expectBinaryOp("1 | 3;", BinaryOp::BIT_OR);
	expectBinaryOp("a && b;", BinaryOp::AND);
	expectBinaryOp("a || b;", BinaryOp::OR);
}

TEST(ParserExpr, LeftAssociativity) {
	// 10 - 4 - 3 must parse as (10 - 4) - 3
	ErrorReporter errors;
	auto expr = parseExpr("10 - 4 - 3;", errors);

	auto *outer = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(outer, nullptr);
	EXPECT_EQ(outer->getOperator(), BinaryOp::SUB);

	auto *inner = expectNode<BinaryExprAST>(outer->getLhs());
	ASSERT_NE(inner, nullptr);
	EXPECT_EQ(inner->getOperator(), BinaryOp::SUB);

	auto *rhs = expectNode<NumberLiteralAST>(outer->getRhs());
	ASSERT_NE(rhs, nullptr);
	EXPECT_DOUBLE_EQ(rhs->getValue(), 3.0);
}

TEST(ParserExpr, MultiplicationBindsTighterThanAddition) {
	// 2 + 3 * 4 must parse as 2 + (3 * 4)
	ErrorReporter errors;
	auto expr = parseExpr("2 + 3 * 4;", errors);

	auto *add = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getOperator(), BinaryOp::ADD);

	auto *lhs = expectNode<NumberLiteralAST>(add->getLhs());
	ASSERT_NE(lhs, nullptr);
	EXPECT_DOUBLE_EQ(lhs->getValue(), 2.0);

	auto *mul = expectNode<BinaryExprAST>(add->getRhs());
	ASSERT_NE(mul, nullptr);
	EXPECT_EQ(mul->getOperator(), BinaryOp::MUL);
}

TEST(ParserExpr, ParensOverridePrecedence) {
	// (2 + 3) * 4 must parse as (2 + 3) * 4
	ErrorReporter errors;
	auto expr = parseExpr("(2 + 3) * 4;", errors);

	auto *mul = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(mul, nullptr);
	EXPECT_EQ(mul->getOperator(), BinaryOp::MUL);

	auto *add = expectNode<BinaryExprAST>(mul->getLhs());
	ASSERT_NE(add, nullptr);
	EXPECT_EQ(add->getOperator(), BinaryOp::ADD);
}

TEST(ParserExpr, BitwisePrecedence) {
	// 1 | 2 ^ 3 & 4 must parse as 1 | (2 ^ (3 & 4))
	ErrorReporter errors;
	auto expr = parseExpr("1 | 2 ^ 3 & 4;", errors);

	auto *bit_or = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(bit_or, nullptr);
	EXPECT_EQ(bit_or->getOperator(), BinaryOp::BIT_OR);

	auto *bit_xor = expectNode<BinaryExprAST>(bit_or->getRhs());
	ASSERT_NE(bit_xor, nullptr);
	EXPECT_EQ(bit_xor->getOperator(), BinaryOp::BIT_XOR);

	auto *bit_and = expectNode<BinaryExprAST>(bit_xor->getRhs());
	ASSERT_NE(bit_and, nullptr);
	EXPECT_EQ(bit_and->getOperator(), BinaryOp::BIT_AND);
}

TEST(ParserExpr, LogicalPrecedence) {
	// a || b && c must parse as a || (b && c)
	ErrorReporter errors;
	auto expr = parseExpr("a || b && c;", errors);

	auto *logical_or = expectNode<BinaryExprAST>(expr.get());
	ASSERT_NE(logical_or, nullptr);
	EXPECT_EQ(logical_or->getOperator(), BinaryOp::OR);

	auto *logical_and = expectNode<BinaryExprAST>(logical_or->getRhs());
	ASSERT_NE(logical_and, nullptr);
	EXPECT_EQ(logical_and->getOperator(), BinaryOp::AND);
}

TEST(ParserExpr, IncompleteExpressionErrors) {
	std::vector<std::string> bad_inputs = {
	    "1 + ;", "2 * ;", "1 << ;", "1 < ;",  "1 == ;", "1 & ;",
	    "1 ^ ;", "1 | ;", "a && ;", "a || ;", "a ? ;"};
	for (const auto &src : bad_inputs) {
		ErrorReporter errors;
		auto expr = parseExpr(src, errors);
		EXPECT_EQ(expr, nullptr) << src;
		EXPECT_TRUE(errors.hasErrors()) << src;
	}
}

TEST(ParserExpr, ArgListTrailingCommaError) {
	ErrorReporter errors;
	auto expr = parseExpr("f(1,);", errors);
	EXPECT_TRUE(errors.hasErrors());
}
