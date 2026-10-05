#pragma once

#include "types/type.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace frontend::sema {

enum class SymbolKind : std::uint8_t {
	Variable,
	Parameter,
	Function,
	Struct,
	Namespace,
};

struct Symbol {
	std::string name;
	const types::Type *type = nullptr;
	SymbolKind kind = SymbolKind::Variable;
	bool used = false;

	bool isConst = false;
	bool isStatic = false;

	size_t line = 0;
	size_t column = 0;
};

struct Scope {
	Scope *parent;
	std::unordered_map<std::string, Symbol> symbols;

	explicit Scope(Scope *parent) : parent(parent) {}
};

class SymbolTable {
	std::vector<std::unique_ptr<Scope>> arena;
	Scope *global = nullptr;
	Scope *current = nullptr;

	[[nodiscard]] const Symbol* searchScope(const Scope*, const std::string&) const;

public:
	void enterScope();
	void exitScope();

	[[nodiscard]] bool declare(Symbol&);

	[[nodiscard]] const Symbol* lookup(const std::string&) const;
	[[nodiscard]] const Symbol* lookupLocal(const std::string&) const;

	[[nodiscard]] const Scope* getGlobalScope();
	[[nodiscard]] const Scope* getCurrentScope();

};

} // namespace frontend::sema
