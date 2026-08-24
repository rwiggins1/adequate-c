#include "semantics/symbol_table.hpp"

namespace frontend::sema {
void SymbolTable::enterScope() {
	arena.push_back(std::make_unique<Scope>(current));
	current = arena.back().get();
}
}
