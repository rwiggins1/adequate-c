#pragma once

#include "gtest/gtest.h"
#include <concepts>
#include <typeinfo>

// Checked downcast for tests: asserts the node is of type T and returns it.
// The derived_from constraint rejects casts across unrelated hierarchies
// at compile time, where a raw dynamic_cast would compile and silently yield
// nullptr at runtime.
template <typename T, typename U>
	requires std::derived_from<T, U>
T *expectNode(U *node) {
	auto *typed = dynamic_cast<T *>(node);
	EXPECT_NE(typed, nullptr)
	    << "expected node of type " << typeid(T).name();
	return typed;
}
