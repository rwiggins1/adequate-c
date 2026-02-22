#include "parser.hpp"
#include "diagnostics/diagnostics.hpp"
#include "lexer/token.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "types/type.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <variant>

namespace frontend {

// !TODO remove lexer.get initialization
Parser::Parser(Lexer &lex, ErrorReporter &errors) 
	: lexer(lex), current(lexer.get()), errors(errors) {}

void Parser::advance() { current = lexer.get(); }

bool Parser::match(TokenType type) { return current.type == type; }

bool Parser::isType(TokenType type) {
	return type == TokenType::INT ||
	       type == TokenType::FLOAT ||
	       type == TokenType::DOUBLE ||
	       type == TokenType::BOOL ||
	       type == TokenType::CHAR ||
	       type == TokenType::STRING ||
	       type == TokenType::VOID ||
	       type == TokenType::INFER;
}

bool Parser::isLiteral(TokenType type) {
	return type == TokenType::NUMBER ||
		type == TokenType::STRING_LIT ||
		type == TokenType::CHAR_LIT ||
		type == TokenType::TRUE ||
		type == TokenType::FALSE;
}

bool Parser::expect(TokenType type) {
	if (!match(type)) {
		std::cerr << "Expected " << static_cast<int>(type)
			  << " but got " << static_cast<int>(current.type)
			  << "\n";
		return false;
	}
	return true;
}


std::unique_ptr<ast::ASTNode> Parser::literal() {
	switch (current.type) {
		case TokenType::NUMBER: {
			auto result = std::make_unique<ast::NumberLiteralAST>(std::stod(current.lexeme));
			advance();
			return result;
		}
		case TokenType::STRING_LIT: {
			auto result = std::make_unique<ast::StringLiteralAST>(std::move(current.lexeme));
			advance();
			return result;
		}
		case TokenType::CHAR_LIT: {
			std::string temp = current.lexeme;
			const char *value = temp.c_str();
			auto result = std::make_unique<ast::CharLiteralAST>(*value);
			advance();
			return result;
		}
		case TokenType::TRUE: {
			auto result = std::make_unique<ast::BoolLiteralAST>(false);
			advance();
			return result;
		}
		case TokenType::FALSE: {
			auto result = std::make_unique<ast::BoolLiteralAST>(false);
			advance();
			return result;
		}
		default:
			errors.error(
                	"Expected literal but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return nullptr;
	}
}

bool Parser::unaryOperator() const {
	return current.type == TokenType::BIT_AND ||
		current.type == TokenType::MULTIPLY ||
		current.type == TokenType::PLUS ||
		current.type == TokenType::MINUS ||
		current.type == TokenType::BIT_NOT ||
		current.type == TokenType::NOT;
}

bool Parser::assignmentOperator() const {
	return current.type == TokenType::EQUAL ||
		current.type == TokenType::MULTIPLY_ASSIGN ||
		current.type == TokenType::DIVIDE_ASSIGN ||
		current.type == TokenType::MODULO_ASSIGN ||
		current.type == TokenType::PLUS_ASSIGN ||
		current.type == TokenType::MINUS_ASSIGN ||
		current.type == TokenType::LEFT_SHIFT_ASSIGN ||
		current.type == TokenType::RIGHT_SHIFT_ASSIGN ||
		current.type == TokenType::BIT_AND_ASSIGN ||
		current.type == TokenType::BIT_XOR_ASSIGN ||
		current.type == TokenType::BIT_OR_ASSIGN;
}

std::unique_ptr<types::Type> Parser::primitiveType() {
	switch (current.type) {
		case TokenType::INT: {
			advance();
			return std::make_unique<types::IntType>();
		}
		case TokenType::STRING: {
			advance();
			return std::make_unique<types::StringType>();
		}
		case TokenType::CHAR: {
			advance();
			return std::make_unique<types::CharType>();
		}
		case TokenType::BOOL: {
			advance();
			return std::make_unique<types::BoolType>();
		}
		case TokenType::FLOAT: {
			advance();
			return std::make_unique<types::FloatType>();
		}
		case TokenType::DOUBLE: {
			advance();
			return std::make_unique<types::DoubleType>();
		}
		case TokenType::VOID: {
			advance();
			return std::make_unique<types::VoidType>();
		}
		default:
			errors.error(
                	"Expected primitive type but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return nullptr;
	}
}

std::unique_ptr<types::Type> Parser::type() {
	switch (current.type) {
		case TokenType::CONST: {
			advance();
			if (auto inner_type = type(); inner_type) { 
				return std::make_unique<types::ConstType>(std::move(inner_type));
			}
			errors.error(
                	"Expected type after 'const' but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return nullptr;
		}
		case TokenType::STATIC: {
			advance();
			if (auto inner_type = type(); inner_type) {
				return std::make_unique<types::StaticType>(std::move(inner_type));
			}
			errors.error(
                	"Expected type after 'static' but got " + current.lexeme,
                	current.line,
                	current.column
            		);
			return nullptr;
		}
		default:
			return primitiveType();
	}
}

std::unique_ptr<ast::ASTNode> Parser::primaryExpr() {
	if (current.type == TokenType::IDENT) {
		auto name = std::make_unique<ast::VariableExprAST>(std::move(current.lexeme));
		advance();
		return name;
	}
	if (auto lit = literal(); lit) {
		return lit;
	}
	if (current.type == TokenType::LBRACE) {
		advance();
		if (auto expr = parseExpression(); expr) {
			advance();
			if (current.type == TokenType::RBRACE) {
				advance();
				return expr;
			}
			errors.error(
			"Expected '}' but got " + current.lexeme,
			current.line,
			current.column
			);
			return nullptr;
		}
	}
	errors.error(
	"Expected identifier or '{' but got " + current.lexeme,
	current.line,
	current.column
	);
	return nullptr;
}

}
