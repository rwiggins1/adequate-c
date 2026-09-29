#include "semantics/symbol_table.hpp"

namespace frontend::sema {
void SymbolTable::enterScope() {
	arena.push_back(std::make_unique<Scope>(current));
	current = arena.back().get();
}

void SymbolTable::exitScope() {
	if (current != global) {
		current = current->parent;
	}
}

[[nodiscard]] bool SymbolTable::declare(Symbol &symbol) {
	auto [it, inserted] =
	    current->symbols.try_emplace(symbol.name, std::move(symbol));
	return inserted;
}

[[nodiscard]] Scope SymbolTable::getGlobalScope() { return *global; }

[[nodiscard]] Scope SymbolTable::getCurrentScope() { return *current; }
} // namespace frontend::sema
