#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/type.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
#include <locale>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>
#include <iostream>

using namespace frontend;
using namespace frontend::ast;
using namespace frontend::types;

TEST(ParserExpr, ParseLiteral) {
	{
	    ErrorReporter errors;
		Lexer lexer("2", errors);

		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto expr_ast =  parser.parseLiteral();

		auto literal_ast = dynamic_cast<ast::NumberLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		double val = literal_ast->getValue();
		ASSERT_EQ(val, 2.0);
	}
	{
		ErrorReporter errors;
		Lexer lexer("'a'", errors);

		Parser parser(lexer, errors);


		// get std::unique_ptr<ExprAST>
		auto expr_ast =  parser.parseLiteral();

		auto literal_ast = dynamic_cast<ast::CharLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		char val = literal_ast->getValue();
		ASSERT_EQ(val, 'a');
	}
	{
		ErrorReporter errors;
		Lexer lexer("\"Hello, World!\"", errors);

		Parser parser(lexer, errors);


		// get std::unique_ptr<ExprAST>
		auto expr_ast =  parser.parseLiteral();

		auto literal_ast = dynamic_cast<ast::StringLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		std::string val = literal_ast->getValue();
		ASSERT_EQ(val, "\"Hello, World!\"");
	}
	{
		ErrorReporter errors;
		Lexer lexer("true", errors);

		Parser parser(lexer, errors);


		// get std::unique_ptr<ExprAST>
		auto expr_ast =  parser.parseLiteral();

		auto literal_ast = dynamic_cast<ast::BoolLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, true);
	}
	{
		ErrorReporter errors;
		Lexer lexer("false", errors);

		Parser parser(lexer, errors);


		// get std::unique_ptr<ExprAST>
		auto expr_ast =  parser.parseLiteral();

		auto literal_ast = dynamic_cast<ast::BoolLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, false);
	}
}

TEST(ParserExpr, ParserPrimitiveType) {
	{
		ErrorReporter errors;
		Lexer lexer("int", errors);
		Parser parser(lexer, errors);


		// get std::unique_ptr<types::Type>
		auto prim_type_ast = parser.parsePrimitiveType();

		auto prim_type = dynamic_cast<types::IntType*>(prim_type_ast.get());
		ASSERT_NE(prim_type, nullptr);

		std::string val = prim_type->toString();
		ASSERT_EQ(val, "int");
	}
}

TEST(ParserExpr, ParserPrimaryExpr) {
	{
		ErrorReporter errors;
		Lexer lexer("2", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto prim_expr_ast = parser.parsePrimaryExpr();

		auto prim_expr = dynamic_cast<ast::NumberLiteralAST*>(prim_expr_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
}

TEST(ParserExpr, ParserPostfixExpr) {
	{
		ErrorReporter errors;
		Lexer lexer("2;", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<ExprAST>
		auto postfix_ast = parser.parsePostfixExpr();

		auto prim_expr = dynamic_cast<ast::NumberLiteralAST*>(postfix_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
}

TEST(ParserExpr, ParserMultiplicative) {
	{
		ErrorReporter errors;
		Lexer lexer("2 * 2;", errors);
		Parser parser(lexer, errors);

		// get std::unique_ptr<types::Type>
		auto mult_ast = parser.parseMultiplicativeExpr();

		auto mult_expr = dynamic_cast<ast::BinaryExprAST*>(mult_ast.get());
		ASSERT_NE(mult_expr, nullptr);

		ast::ExprAST* lhs_expr = mult_expr->getLhs();
		auto lhs = dynamic_cast<ast::NumberLiteralAST*>(lhs_expr);
		ASSERT_NE(lhs, nullptr);

		BinaryOp op = mult_expr->getOperator();
		ASSERT_EQ(op, BinaryOp::MUL);

		ast::ExprAST* rhs_expr = mult_expr->getRhs();
		auto rhs = dynamic_cast<ast::NumberLiteralAST*>(rhs_expr);
		ASSERT_NE(rhs, nullptr);
	}
}
