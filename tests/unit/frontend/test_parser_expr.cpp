#include "frontend/ast/ast.hpp"
#include "frontend/ast/decl.hpp"
#include "frontend/ast/expr.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/diagnostics/diagnostics.hpp"
#include "frontend/lexer/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/types/type.hpp"
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
		Lexer lexer("2");
		ErrorReporter errors;

		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto expr_opt =  parser.parseLiteral();
		ASSERT_TRUE(expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto expr_ast = std::move(*expr_opt);

		auto literal_ast = dynamic_cast<ast::NumberLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		double val = literal_ast->getValue();
		ASSERT_EQ(val, 2.0);
	}
	{
		Lexer lexer("'a'");
		ErrorReporter errors;

		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto expr_opt =  parser.parseLiteral();
		ASSERT_TRUE(expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto expr_ast = std::move(*expr_opt);

		auto literal_ast = dynamic_cast<ast::CharLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		char val = literal_ast->getValue();
		ASSERT_EQ(val, 'a');
	}
	{
		Lexer lexer("\"Hello, World!\"");
		ErrorReporter errors;

		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto expr_opt =  parser.parseLiteral();
		ASSERT_TRUE(expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto expr_ast = std::move(*expr_opt);

		auto literal_ast = dynamic_cast<ast::StringLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		std::string val = literal_ast->getValue();
		ASSERT_EQ(val, "\"Hello, World!\"");
	}
	{
		Lexer lexer("true");
		ErrorReporter errors;

		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto expr_opt =  parser.parseLiteral();
		ASSERT_TRUE(expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto expr_ast = std::move(*expr_opt);

		auto literal_ast = dynamic_cast<ast::BoolLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, true);
	}
	{
		Lexer lexer("false");
		ErrorReporter errors;

		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto expr_opt =  parser.parseLiteral();
		ASSERT_TRUE(expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto expr_ast = std::move(*expr_opt);

		auto literal_ast = dynamic_cast<ast::BoolLiteralAST*>(expr_ast.get());
		ASSERT_NE(literal_ast, nullptr);

		bool val = literal_ast->getValue();
		ASSERT_EQ(val, false);
	}
}

TEST(ParserExpr, ParserPrimitiveType) {
	{
		Lexer lexer("int");
		ErrorReporter errors;
		Parser parser(lexer, errors);
		
		// get std::optional<std::unique_ptr<types::Type>>
		auto prim_type_opt = parser.parsePrimitiveType();
		ASSERT_TRUE(prim_type_opt.has_value());

		// get std::unique_ptr<types::Type>
		auto prim_type_ast = std::move(*prim_type_opt);

		auto prim_type = dynamic_cast<types::IntType*>(prim_type_ast.get());
		ASSERT_NE(prim_type, nullptr);
	
		std::string val = prim_type->toString();
		ASSERT_EQ(val, "int");
	}
}

TEST(ParserExpr, ParserPrimaryExpr) {
	{
		Lexer lexer("2");
		ErrorReporter errors;
		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto prim_expr_opt = parser.parsePrimaryExpr();
		errors.printAll();
		ASSERT_TRUE(prim_expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto prim_expr_ast = std::move(*prim_expr_opt);

		auto prim_expr = dynamic_cast<ast::NumberLiteralAST*>(prim_expr_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
}

TEST(ParserExpr, ParserPostfixExpr) {
	{
		Lexer lexer("2;");
		ErrorReporter errors;
		Parser parser(lexer, errors);

		// get std::optional<std::unique_ptr<ExprAST>>
		auto postfix_expr_opt = parser.parsePostfixExpr();
		errors.printAll();
		ASSERT_TRUE(postfix_expr_opt.has_value());

		// get std::unique_ptr<ExprAST>
		auto postfix_ast = std::move(*postfix_expr_opt);

		auto prim_expr = dynamic_cast<ast::NumberLiteralAST*>(postfix_ast.get());
		ASSERT_NE(prim_expr, nullptr);
	}
}

TEST(ParserExpr, ParserMultiplicative) {
	{
		Lexer lexer("2 * 2;");
		ErrorReporter errors;
		Parser parser(lexer, errors);
		
		// get std::optional<std::unique_ptr<ExprAST>>
		auto mult_opt = parser.parseMultiplicativeExpr();
		errors.printAll();
		ASSERT_TRUE(mult_opt.has_value());

		// get std::unique_ptr<types::Type>
		auto mult_ast = std::move(*mult_opt);

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

