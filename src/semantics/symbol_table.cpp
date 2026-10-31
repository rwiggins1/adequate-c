#include "semantics/symbol_table.hpp"
#include <memory>
#include <string>

namespace frontend::sema {

SymbolTable::SymbolTable() {
	enterScope();
	global = current;

	Symbol print_built_in{.name = "print", .kind = SymbolKind::Function};
	global->symbols.emplace("print", std::move(print_built_in));
}

void SymbolTable::enterScope() {
	arena.push_back(std::make_unique<Scope>(current));
	current = arena.back().get();
}

void SymbolTable::exitScope() {
	if (current != global) {
		current = current->parent;
	}
}

bool SymbolTable::declare(Symbol &symbol) {
	auto [it, inserted] =
	    current->symbols.try_emplace(symbol.name, std::move(symbol));
	return inserted;
}

const Symbol *SymbolTable::lookup(const std::string &symbol_name) const {
	for (const Scope *scope = current; scope != nullptr;
	     scope = scope->parent) {
		if (const Symbol *found = searchScope(scope, symbol_name)) {
			return found;
		}
	}
	return nullptr;
}

const Symbol *SymbolTable::lookupLocal(const std::string &symbol_name) const {
	return searchScope(current, symbol_name);
}

const Symbol *SymbolTable::searchScope(const Scope *scope,
				       const std::string &symbol_name) {
	auto it = scope->symbols.find(symbol_name);
	if (it == scope->symbols.end()) {
		return nullptr;
	}
	return &it->second;
}

const Scope *SymbolTable::getGlobalScope() const noexcept { return global; }

const Scope *SymbolTable::getCurrentScope() const noexcept { return current; }
} // namespace frontend::sema
