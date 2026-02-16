#include <memory>
#include <string>

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
	[[nodiscard]] Type* getElementType() const { return elementType.get();} 
	[[nodiscard]] size_t getSize() const { return size;} 

	[[nodiscard]] bool isPrimitive() const override { return false; }
};

class StructType : public Type {
	std::string name;
public:
	[[nodiscard]] bool isPrimitive() const override { return false; }
};

}
