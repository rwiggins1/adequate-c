#include "../test_helpers.hpp"
#include "ast/ast.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "types/type.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

using namespace frontend::ast;
using namespace frontend::types;

TEST(astTest, NumberExpr) {
	auto expr = std::make_unique<NumberLiteralAST>(42.0);
	ASSERT_NE(expr, nullptr);
	ASSERT_EQ(expr->getValue(), 42.0);
}

TEST(astTest, StringLiteral) {
	auto str_lit = std::make_unique<StringLiteralAST>("Hello, World!");
	ASSERT_NE(str_lit, nullptr);
	ASSERT_EQ(str_lit->getValue(), "Hello, World!");
}

TEST(astTest, CharLiteral) {
	auto char_lit = std::make_unique<CharLiteralAST>('a');
	ASSERT_NE(char_lit, nullptr);
	ASSERT_EQ(char_lit->getValue(), 'a');
}

TEST(astTest, BoolLiteral) {
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	ASSERT_NE(bool_lit, nullptr);
	ASSERT_EQ(bool_lit->getValue(), true);
}

TEST(astTest, UnaryExpr) {
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	auto unary =
	    std::make_unique<UnaryExprAST>(UnaryOp::NOT, std::move(bool_lit));
	ASSERT_EQ(unary->getOperator(), UnaryOp::NOT);

	ExprAST *literal = unary->getOperand();
	auto *boolLiteral = expectNode<BoolLiteralAST>(literal);

	ASSERT_EQ(boolLiteral->getValue(), true);
}

// 2 + 3
TEST(astTest, BinaryExpr) {
	auto two = std::make_unique<NumberLiteralAST>(2);
	auto three = std::make_unique<NumberLiteralAST>(3);
	auto add = std::make_unique<BinaryExprAST>(
	    BinaryOp::ADD, std::move(two), std::move(three));
	ASSERT_EQ(add->getOperator(), BinaryOp::ADD);

	ExprAST *lhs = add->getLhs();
	auto *lhsLiteral = expectNode<NumberLiteralAST>(lhs);
	ASSERT_EQ(lhsLiteral->getValue(), 2);

	ExprAST *rhs = add->getRhs();
	auto *rhsLiteral = expectNode<NumberLiteralAST>(rhs);

	ASSERT_EQ(rhsLiteral->getValue(), 3);
}

// someFunc(12, "test", true)
TEST(astTest, FunctionCallExpr) {
	auto callee = std::make_unique<VariableExprAST>("someFunc");
	auto twelve = std::make_unique<NumberLiteralAST>(12);
	auto testStr = std::make_unique<StringLiteralAST>("test");
	auto bool_lit = std::make_unique<BoolLiteralAST>(true);
	std::vector<std::unique_ptr<ExprAST>> args;

	args.push_back(std::move(twelve));
	args.push_back(std::move(testStr));
	args.push_back(std::move(bool_lit));

	auto funcCall =
	    std::make_unique<CallExprAST>(std::move(callee), std::move(args));
	const auto &Args = funcCall->getArgs();

	auto *TwelveLiteral = expectNode<NumberLiteralAST>(Args[0].get());

	ASSERT_EQ(TwelveLiteral->getValue(), 12);
}

// cond ? 1 : 2
TEST(astTest, TernaryExpr) {
	auto cond = std::make_unique<BoolLiteralAST>(true);
	auto then_branch = std::make_unique<NumberLiteralAST>(1);
	auto else_branch = std::make_unique<NumberLiteralAST>(2);

	auto ternary = std::make_unique<TernaryExprAST>(
	    std::move(cond), std::move(then_branch), std::move(else_branch));

	auto *cond_check = expectNode<BoolLiteralAST>(ternary->getCondition());
	ASSERT_NE(cond_check, nullptr);
	ASSERT_EQ(cond_check->getValue(), true);

	auto *then_check =
	    expectNode<NumberLiteralAST>(ternary->getThenBranch());
	ASSERT_NE(then_check, nullptr);
	ASSERT_EQ(then_check->getValue(), 1.0);

	auto *else_check =
	    expectNode<NumberLiteralAST>(ternary->getElseBranch());
	ASSERT_NE(else_check, nullptr);
	ASSERT_EQ(else_check->getValue(), 2.0);
}

TEST(astTest, ReturnStmt) {
	auto value = std::make_unique<NumberLiteralAST>(7);
	auto ret = std::make_unique<ReturnStmtAST>(std::move(value));

	auto *value_check = expectNode<NumberLiteralAST>(ret->getValue());
	ASSERT_NE(value_check, nullptr);
	ASSERT_EQ(value_check->getValue(), 7.0);

	// return without a value
	auto bare_ret = std::make_unique<ReturnStmtAST>(nullptr);
	ASSERT_EQ(bare_ret->getValue(), nullptr);
}

TEST(astTest, BlockStmt) {
	std::vector<std::unique_ptr<StmtAST>> stmts;
	stmts.emplace_back(std::make_unique<BreakStmtAST>());
	stmts.emplace_back(std::make_unique<ContinueStmtAST>());

	auto block = std::make_unique<BlockStmtAST>(std::move(stmts));
	const auto &block_stmts = block->getStmts();
	ASSERT_EQ(block_stmts.size(), 2);

	ASSERT_NE(expectNode<BreakStmtAST>(block_stmts[0].get()), nullptr);
	ASSERT_NE(expectNode<ContinueStmtAST>(block_stmts[1].get()), nullptr);
}

// if (true) { break; } else { continue; }
TEST(astTest, IfStmt) {
	auto cond = std::make_unique<BoolLiteralAST>(true);

	std::vector<std::unique_ptr<StmtAST>> then_stmts;
	then_stmts.emplace_back(std::make_unique<BreakStmtAST>());
	auto then_block = std::make_unique<BlockStmtAST>(std::move(then_stmts));

	std::vector<std::unique_ptr<StmtAST>> else_stmts;
	else_stmts.emplace_back(std::make_unique<ContinueStmtAST>());
	auto else_block = std::make_unique<BlockStmtAST>(std::move(else_stmts));

	auto if_stmt = std::make_unique<IfStmtAST>(
	    std::move(cond), std::move(then_block), std::move(else_block));
	ASSERT_NE(if_stmt, nullptr);

	// else branch defaults to nullptr
	auto if_no_else = std::make_unique<IfStmtAST>(
	    std::make_unique<BoolLiteralAST>(false),
	    std::make_unique<BlockStmtAST>(
		std::vector<std::unique_ptr<StmtAST>>{}));
	ASSERT_NE(if_no_else, nullptr);
}

// while (true) { break; }
TEST(astTest, WhileStmt) {
	auto cond = std::make_unique<BoolLiteralAST>(true);

	std::vector<std::unique_ptr<StmtAST>> body_stmts;
	body_stmts.emplace_back(std::make_unique<BreakStmtAST>());
	auto body = std::make_unique<BlockStmtAST>(std::move(body_stmts));

	auto while_stmt =
	    std::make_unique<WhileStmtAST>(std::move(cond), std::move(body));
	ASSERT_NE(while_stmt, nullptr);
}

// for (int i = 0; i < 10; i++) {}
TEST(astTest, ForStmt) {
	auto init = std::make_unique<DeclStmtAST>(
	    std::make_unique<VariableDeclarationAST>(
		std::make_unique<IntType>(), "i",
		std::make_unique<NumberLiteralAST>(0)));

	auto cond = std::make_unique<BinaryExprAST>(
	    BinaryOp::LT, std::make_unique<VariableExprAST>("i"),
	    std::make_unique<NumberLiteralAST>(10));

	auto update = std::make_unique<UnaryExprAST>(
	    UnaryOp::POST_INCREMENT, std::make_unique<VariableExprAST>("i"));

	auto body = std::make_unique<BlockStmtAST>(
	    std::vector<std::unique_ptr<StmtAST>>{});

	auto for_stmt =
	    std::make_unique<ForStmtAST>(std::move(init), std::move(cond),
					 std::move(update), std::move(body));
	ASSERT_NE(for_stmt, nullptr);
}

TEST(astTest, VarDec) {
	auto expr = std::make_unique<NumberLiteralAST>(42.0);
	auto integer_type = std::make_unique<frontend::types::IntType>();
	auto var_dec = std::make_unique<VariableDeclarationAST>(
	    std::move(integer_type), "x", nullptr, std::move(expr));
	ASSERT_NE(var_dec, nullptr);
	ASSERT_NE(var_dec->getType(), nullptr);
	ASSERT_EQ(var_dec->getName(), "x");
	ASSERT_NE(var_dec->getInit(), nullptr);
	ASSERT_EQ(var_dec->hasInit(), true);
}

TEST(astTest, Comprehensive) {
	// ========== BUILD ADD FUNCTION ==========
	auto add_a = std::make_unique<VariableExprAST>("a");
	auto add_b = std::make_unique<VariableExprAST>("b");
	auto a_plus_b = std::make_unique<BinaryExprAST>(
	    BinaryOp::ADD, std::move(add_a), std::move(add_b));
	auto add_return_stmt =
	    std::make_unique<ReturnStmtAST>(std::move(a_plus_b));

	std::vector<std::pair<std::unique_ptr<Type>, std::string>> add_params;
	add_params.push_back({std::make_unique<IntType>(), "a"});
	add_params.push_back({std::make_unique<IntType>(), "b"});

	auto add_return_type = std::make_unique<IntType>();
	auto add_proto = std::make_unique<PrototypeAST>(
	    "add", std::move(add_params), std::move(add_return_type));

	std::vector<std::unique_ptr<StmtAST>> add_body;
	add_body.emplace_back(std::move(add_return_stmt));
	auto add_func = std::make_unique<FunctionAST>(
	    std::move(add_proto),
	    std::make_unique<BlockStmtAST>(std::move(add_body)));

	// ========== BUILD MULTIPLY FUNCTION ==========
	auto mul_a = std::make_unique<VariableExprAST>("a");
	auto mul_b = std::make_unique<VariableExprAST>("b");
	auto a_mul_b = std::make_unique<BinaryExprAST>(
	    BinaryOp::MUL, std::move(mul_a), std::move(mul_b));
	auto return_stmt = std::make_unique<ReturnStmtAST>(std::move(a_mul_b));

	std::vector<std::pair<std::unique_ptr<Type>, std::string>> mul_params;
	mul_params.push_back({std::make_unique<IntType>(), "a"});
	mul_params.push_back({std::make_unique<IntType>(), "b"});

	auto mul_return_type = std::make_unique<IntType>();
	auto mul_proto = std::make_unique<PrototypeAST>(
	    "multiply", std::move(mul_params), std::move(mul_return_type));

	std::vector<std::unique_ptr<StmtAST>> mul_body;
	mul_body.push_back(std::move(return_stmt));
	auto multiply_func = std::make_unique<FunctionAST>(
	    std::move(mul_proto),
	    std::make_unique<BlockStmtAST>(std::move(mul_body)));

	// ========== BUILD MATH NAMESPACE ==========
	std::vector<std::unique_ptr<DeclAST>> decls;
	decls.emplace_back(std::move(add_func));
	decls.emplace_back(std::move(multiply_func));
	auto math_namespace =
	    std::make_unique<NamespaceAST>("math", std::move(decls));

	// ===== TEST NAMESPACE =====
	ASSERT_NE(math_namespace, nullptr);
	EXPECT_EQ(math_namespace->getName(), "math");

	const auto &namespace_decls = math_namespace->getDeclarations();
	ASSERT_EQ(namespace_decls.size(), 2)
	    << "Namespace should contain 2 functions";

	// Test first function (add)
	auto *add_func_ptr = expectNode<FunctionAST>(namespace_decls[0].get());
	ASSERT_NE(add_func_ptr, nullptr)
	    << "First declaration should be a function";
	EXPECT_EQ(add_func_ptr->getProto()->getName(), "add");

	const auto &add_params_check = add_func_ptr->getProto()->getParams();
	ASSERT_EQ(add_params_check.size(), 2)
	    << "add() should have 2 parameters";
	EXPECT_EQ(add_params_check[0].second, "a")
	    << "First param should be 'a'";
	EXPECT_EQ(add_params_check[1].second, "b")
	    << "Second param should be 'b'";
	EXPECT_EQ(add_params_check[0].first->toString(), "int")
	    << "First param should be int";
	EXPECT_EQ(add_params_check[1].first->toString(), "int")
	    << "Second param should be int";
	EXPECT_EQ(add_func_ptr->getProto()->getReturnType()->toString(), "int");

	ASSERT_NE(add_func_ptr->getBody(), nullptr)
	    << "add() should have a body";
	const auto &add_body_check = add_func_ptr->getBody()->getStmts();
	ASSERT_EQ(add_body_check.size(), 1)
	    << "add() body should have 1 statement";

	auto *add_return = expectNode<ReturnStmtAST>(add_body_check[0].get());
	ASSERT_NE(add_return, nullptr) << "First statement should be return";
	ASSERT_NE(add_return->getValue(), nullptr)
	    << "Return should have a value";

	auto *add_binary = expectNode<BinaryExprAST>(add_return->getValue());
	ASSERT_NE(add_binary, nullptr)
	    << "Return value should be binary expression";
	EXPECT_EQ(add_binary->getOperator(), BinaryOp::ADD)
	    << "Should be ADD operation";

	auto *add_left = expectNode<VariableExprAST>(add_binary->getLhs());
	auto *add_right = expectNode<VariableExprAST>(add_binary->getRhs());
	ASSERT_NE(add_left, nullptr) << "Left operand should be variable";
	ASSERT_NE(add_right, nullptr) << "Right operand should be variable";
	EXPECT_EQ(add_left->getName(), "a");
	EXPECT_EQ(add_right->getName(), "b");

	// Test second function (multiply)
	auto *mul_func_ptr = expectNode<FunctionAST>(namespace_decls[1].get());
	ASSERT_NE(mul_func_ptr, nullptr)
	    << "Second declaration should be a function";
	EXPECT_EQ(mul_func_ptr->getProto()->getName(), "multiply");

	const auto &mul_params_check = mul_func_ptr->getProto()->getParams();
	ASSERT_EQ(mul_params_check.size(), 2)
	    << "multiply() should have 2 parameters";
	EXPECT_EQ(mul_params_check[0].second, "a");
	EXPECT_EQ(mul_params_check[1].second, "b");

	ASSERT_NE(mul_func_ptr->getBody(), nullptr)
	    << "multiply() should have a body";
	const auto &mul_body_check = mul_func_ptr->getBody()->getStmts();
	ASSERT_EQ(mul_body_check.size(), 1)
	    << "multiply() body should have 1 statement";

	auto *mul_return = expectNode<ReturnStmtAST>(mul_body_check[0].get());
	ASSERT_NE(mul_return, nullptr);

	auto *mul_binary = expectNode<BinaryExprAST>(mul_return->getValue());
	ASSERT_NE(mul_binary, nullptr);
	EXPECT_EQ(mul_binary->getOperator(), BinaryOp::MUL)
	    << "Should be MUL operation";

	// ========== BUILD STRUCT ==========
	auto x = std::make_unique<VariableDeclarationAST>(
	    std::make_unique<IntType>(), "x");
	auto y = std::make_unique<VariableDeclarationAST>(
	    std::make_unique<IntType>(), "y");

	std::vector<std::unique_ptr<VariableDeclarationAST>> fields;
	fields.push_back(std::move(x));
	fields.push_back(std::move(y));

	auto point_struct =
	    std::make_unique<StructAST>("Point", std::move(fields));

	// ===== TEST STRUCT =====
	ASSERT_NE(point_struct, nullptr);
	EXPECT_EQ(point_struct->getName(), "Point");

	const auto &struct_fields = point_struct->getFields();
	ASSERT_EQ(struct_fields.size(), 2) << "Struct should have 2 fields";
	EXPECT_EQ(struct_fields[0]->getName(), "x");
	EXPECT_EQ(struct_fields[1]->getName(), "y");
	EXPECT_EQ(struct_fields[0]->getType()->toString(), "int");
	EXPECT_EQ(struct_fields[1]->getType()->toString(), "int");

	// ========== BUILD MAIN FUNCTION VARIABLES ==========
	// Call: add(5, 3)
	std::vector<std::unique_ptr<ExprAST>> args;
	args.emplace_back(std::make_unique<NumberLiteralAST>(5));
	args.emplace_back(std::make_unique<NumberLiteralAST>(3));

	auto sum_callee = std::make_unique<VariableExprAST>("add");
	auto sum_call = std::make_unique<CallExprAST>(std::move(sum_callee),
						      std::move(args));

	auto sum = std::make_unique<VariableDeclarationAST>(
	    std::make_unique<IntType>(), "sum", nullptr, std::move(sum_call));

	// ===== TEST SUM VARIABLE =====
	ASSERT_NE(sum, nullptr);
	EXPECT_EQ(sum->getName(), "sum");
	EXPECT_EQ(sum->getType()->toString(), "int");
	ASSERT_NE(sum->getInit(), nullptr) << "sum should have initializer";

	auto *sum_call_check = expectNode<CallExprAST>(sum->getInit());
	ASSERT_NE(sum_call_check, nullptr)
	    << "Initializer should be function call";

	auto sum_callee_check = sum_call_check->getCallee();
	auto sum_calle =
	    expectNode<VariableExprAST>(std::move(sum_callee_check));
	EXPECT_EQ(sum_calle->getName(), "add");

	const auto &sum_args = sum_call_check->getArgs();
	ASSERT_EQ(sum_args.size(), 2) << "add() call should have 2 arguments";

	auto *sum_arg1 = expectNode<NumberLiteralAST>(sum_args[0].get());
	auto *sum_arg2 = expectNode<NumberLiteralAST>(sum_args[1].get());
	ASSERT_NE(sum_arg1, nullptr);
	ASSERT_NE(sum_arg2, nullptr);
	EXPECT_DOUBLE_EQ(sum_arg1->getValue(), 5.0);
	EXPECT_DOUBLE_EQ(sum_arg2->getValue(), 3.0);

	// Call: multiply(4, 7)
	std::vector<std::unique_ptr<ExprAST>> args2;
	args2.emplace_back(std::make_unique<NumberLiteralAST>(4));
	args2.emplace_back(std::make_unique<NumberLiteralAST>(7));

	auto mul_var = std::make_unique<VariableExprAST>("multiply");

	auto mul_call =
	    std::make_unique<CallExprAST>(std::move(mul_var), std::move(args2));

	auto product = std::make_unique<VariableDeclarationAST>(
	    std::make_unique<IntType>(), "product", nullptr,
	    std::move(mul_call));

	// ===== TEST PRODUCT VARIABLE =====
	ASSERT_NE(product, nullptr);
	EXPECT_EQ(product->getName(), "product");
	EXPECT_EQ(product->getType()->toString(), "int");
	ASSERT_NE(product->getInit(), nullptr);

	auto *prod_call_check = expectNode<CallExprAST>(product->getInit());
	ASSERT_NE(prod_call_check, nullptr);

	auto prod_callee_check = prod_call_check->getCallee();
	auto prod_callee_var = expectNode<VariableExprAST>(prod_callee_check);
	std::string prod_callee = prod_callee_var->getName();
	EXPECT_EQ(prod_callee, "multiply");

	const auto &prod_args = prod_call_check->getArgs();
	ASSERT_EQ(prod_args.size(), 2);

	auto *prod_arg1 = expectNode<NumberLiteralAST>(prod_args[0].get());
	auto *prod_arg2 = expectNode<NumberLiteralAST>(prod_args[1].get());
	ASSERT_NE(prod_arg1, nullptr);
	ASSERT_NE(prod_arg2, nullptr);
	EXPECT_DOUBLE_EQ(prod_arg1->getValue(), 4.0);
	EXPECT_DOUBLE_EQ(prod_arg2->getValue(), 7.0);

	// Struct variable
	auto p = std::make_unique<VariableDeclarationAST>(
	    std::make_unique<StructType>("Point"), "p");

	// ===== TEST STRUCT VARIABLE =====
	ASSERT_NE(p, nullptr);
	EXPECT_EQ(p->getName(), "p");
	EXPECT_EQ(p->getType()->toString(), "struct Point");
	EXPECT_EQ(p->getInit(), nullptr) << "p should have no initializer";

	// Assignment statements
	auto px = std::make_unique<AssignmentStmtAST>(
	    "p", AssignOp::ASSIGN, std::make_unique<VariableExprAST>("sum"));
	auto py = std::make_unique<AssignmentStmtAST>(
	    "y", AssignOp::ASSIGN,
	    std::make_unique<VariableExprAST>("product"));

	// ===== TEST ASSIGNMENTS =====
	ASSERT_NE(px, nullptr);
	EXPECT_EQ(px->getVariableName(), "p");
	auto *px_value = expectNode<VariableExprAST>(px->getValue());
	ASSERT_NE(px_value, nullptr);
	EXPECT_EQ(px_value->getName(), "sum");

	ASSERT_NE(py, nullptr);
	EXPECT_EQ(py->getVariableName(), "y");
	auto *py_value = expectNode<VariableExprAST>(py->getValue());
	ASSERT_NE(py_value, nullptr);
	EXPECT_EQ(py_value->getName(), "product");

	// ===== SUMMARY =====
	std::cout
	    << "\n✅ All AST nodes constructed and validated successfully!\n";
	std::cout << "   - Namespace 'math' with 2 functions (add, multiply)\n";
	std::cout << "   - Struct 'Point' with 2 fields (x, y)\n";
	std::cout << "   - 3 variables (sum, product, p)\n";
	std::cout << "   - 2 assignments (p = sum, y = product)\n";
}
