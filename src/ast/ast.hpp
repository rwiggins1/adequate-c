#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace frontend::ast {

class ASTVisitor;

/// A possibly namespace-qualified identifier, e.g. `math::vec::dot`.
/// `qualifiers` holds the leading segments ({"math", "vec"}) and `name`
/// the final one ("dot"); unqualified names have empty `qualifiers`.
struct QualifiedName {
	std::vector<std::string> qualifiers;
	std::string name;

	QualifiedName() = default;
	QualifiedName(std::string name) : name(std::move(name)) {}
	QualifiedName(std::vector<std::string> qualifiers, std::string name)
	    : qualifiers(std::move(qualifiers)), name(std::move(name)) {}

	[[nodiscard]] bool isQualified() const noexcept {
		return !qualifiers.empty();
	}

	/// "a::b::name" form for diagnostics and mangling.
	[[nodiscard]] std::string str() const {
		std::string out;
		for (const auto &q : qualifiers) {
			out += q;
			out += "::";
		}
		out += name;
		return out;
	}
};

// Base Class
class ASTNode {
public:
	virtual ~ASTNode() = default;

	ASTNode(const ASTNode &) = delete;
	ASTNode &operator=(const ASTNode &) = delete;

	ASTNode(ASTNode &&) = default;
	ASTNode &operator=(ASTNode &&) = default;

	virtual void accept(ASTVisitor &V) = 0;

protected:
	ASTNode() = default;
};

// Expressions
class ExprAST : public ASTNode {
protected:
	ExprAST() = default;
};

// Statements
class StmtAST : public ASTNode {};

// Declarations
class DeclAST : public ASTNode {};

} // namespace frontend::ast
