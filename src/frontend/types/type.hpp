#pragma once
#include <memory>
#include <string>
#include <string_view>

namespace frontend::types {

class Type {
public:
    virtual ~Type() = default;
    
    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] virtual bool equals(const Type* other) const = 0;
    
    // Type predicates
    [[nodiscard]] virtual bool isPrimitive() const { return false; }
    [[nodiscard]] virtual bool isArray() const { return false; }
    [[nodiscard]] virtual bool isStruct() const { return false; }
    [[nodiscard]] virtual bool isNullable() const { return false; }
    [[nodiscard]] virtual bool isFunction() const { return false; }
    
protected:
    Type() = default;
};

// Primitive types
class IntType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "int"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const IntType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class FloatType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "float"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const FloatType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class DoubleType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "double"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const DoubleType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class BoolType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "bool"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const BoolType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class StringType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "string"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const StringType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class VoidType : public Type {
public:
	[[nodiscard]] std::string toString() const override { return "void"; }
	[[nodiscard]] bool equals(const Type* other) const override {
		return dynamic_cast<const VoidType*>(other) != nullptr;
	}

	[[nodiscard]] bool isPrimitive() const override { return true; }
};

class ArrayType : public Type {
	std::unique_ptr<Type> elementType;
	size_t size;
public:
	ArrayType(std::unique_ptr<Type> elem, size_t sz)
        	: elementType(std::move(elem)), size(sz) {}

	[[nodiscard]] std::string toString() const override {
        	return elementType->toString() + "[" + std::to_string(size) + "]";
    	}

	[[nodiscard]] Type* getElementType() const { return elementType.get();} 
	[[nodiscard]] size_t getSize() const { return size;} 
	bool equals(const Type* other) const override {
		auto arr = dynamic_cast<const ArrayType*>(other);
		if (!arr) { 
			return false; 
		}
		return size == arr->size && elementType->equals(arr->elementType.get());
	}
	[[nodiscard]] bool isPrimitive() const override { return false; }
};

class StructType : public Type {
	std::string name;
public:
	StructType(std::string n) : name(std::move(n)) {}

	bool equals(const Type* other) const override {
        	auto s = dynamic_cast<const StructType*>(other);
        	return s && s->name == name;
    	}
	[[nodiscard]] std::string_view getName() const { return name; }
	[[nodiscard]] bool isPrimitive() const override { return false; }
};

}
