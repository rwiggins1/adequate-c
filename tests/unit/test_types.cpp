
#include "types/type.hpp"
#include "gtest/gtest.h"
#include <memory>
#include <string>
#include <vector>

using namespace frontend;

TEST(Types, IntegerType) {
	auto int_type = std::make_unique<types::IntType>();
	ASSERT_NE(int_type, nullptr);

	ASSERT_EQ(int_type->toString(), "int");

	// test equals()
	auto type = std::make_unique<types::FloatType>();
	ASSERT_FALSE(int_type->equals(type.get()));

	// check if primitive type
	ASSERT_TRUE(int_type->isPrimitive());
}

TEST(Types, PrimitiveToStringAndPredicates) {
	std::vector<std::pair<std::unique_ptr<types::Type>, std::string>>
	    cases;
	cases.emplace_back(std::make_unique<types::IntType>(), "int");
	cases.emplace_back(std::make_unique<types::FloatType>(), "float");
	cases.emplace_back(std::make_unique<types::DoubleType>(), "double");
	cases.emplace_back(std::make_unique<types::BoolType>(), "bool");
	cases.emplace_back(std::make_unique<types::CharType>(), "char");
	cases.emplace_back(std::make_unique<types::StringType>(), "string");
	cases.emplace_back(std::make_unique<types::VoidType>(), "void");

	for (const auto &[type, name] : cases) {
		EXPECT_EQ(type->toString(), name);
		EXPECT_TRUE(type->isPrimitive()) << name;
		EXPECT_FALSE(type->isArray()) << name;
		EXPECT_FALSE(type->isStruct()) << name;
		EXPECT_FALSE(type->isNullable()) << name;
		EXPECT_FALSE(type->isFunction()) << name;
	}
}

TEST(Types, PrimitiveEquals) {
	std::vector<std::unique_ptr<types::Type>> all;
	all.emplace_back(std::make_unique<types::IntType>());
	all.emplace_back(std::make_unique<types::FloatType>());
	all.emplace_back(std::make_unique<types::DoubleType>());
	all.emplace_back(std::make_unique<types::BoolType>());
	all.emplace_back(std::make_unique<types::CharType>());
	all.emplace_back(std::make_unique<types::StringType>());
	all.emplace_back(std::make_unique<types::VoidType>());

	std::vector<std::unique_ptr<types::Type>> same;
	same.emplace_back(std::make_unique<types::IntType>());
	same.emplace_back(std::make_unique<types::FloatType>());
	same.emplace_back(std::make_unique<types::DoubleType>());
	same.emplace_back(std::make_unique<types::BoolType>());
	same.emplace_back(std::make_unique<types::CharType>());
	same.emplace_back(std::make_unique<types::StringType>());
	same.emplace_back(std::make_unique<types::VoidType>());

	for (size_t i = 0; i < all.size(); i++) {
		for (size_t j = 0; j < all.size(); j++) {
			EXPECT_EQ(all[i]->equals(same[j].get()), i == j)
			    << all[i]->toString() << " vs "
			    << same[j]->toString();
		}
	}
}

TEST(Types, ArrayType) {
	types::ArrayType int3(std::make_unique<types::IntType>(), 3);

	EXPECT_EQ(int3.toString(), "int[3]");
	EXPECT_EQ(int3.getSize(), 3);
	ASSERT_NE(int3.getElementType(), nullptr);
	EXPECT_TRUE(int3.getElementType()->isPrimitive());
	EXPECT_FALSE(int3.isPrimitive());

	types::ArrayType int3_same(std::make_unique<types::IntType>(), 3);
	types::ArrayType int5(std::make_unique<types::IntType>(), 5);
	types::ArrayType bool3(std::make_unique<types::BoolType>(), 3);
	types::IntType plain_int;

	EXPECT_TRUE(int3.equals(&int3_same));
	EXPECT_FALSE(int3.equals(&int5)) << "same element, different size";
	EXPECT_FALSE(int3.equals(&bool3)) << "same size, different element";
	EXPECT_FALSE(int3.equals(&plain_int));
}

TEST(Types, StructType) {
	types::StructType point("Point");

	EXPECT_EQ(point.toString(), "struct Point");
	EXPECT_EQ(point.getName(), "Point");
	EXPECT_TRUE(point.isStruct());
	EXPECT_FALSE(point.isPrimitive());

	types::StructType point_same("Point");
	types::StructType other("Other");
	types::IntType plain_int;

	EXPECT_TRUE(point.equals(&point_same));
	EXPECT_FALSE(point.equals(&other));
	EXPECT_FALSE(point.equals(&plain_int));
}

TEST(Types, ConstType) {
	types::ConstType const_int(std::make_unique<types::IntType>());

	EXPECT_EQ(const_int.toString(), "const int");
	ASSERT_NE(const_int.getInnerType(), nullptr);
	EXPECT_EQ(const_int.getInnerType()->toString(), "int");

	types::ConstType const_int_same(std::make_unique<types::IntType>());
	types::ConstType const_bool(std::make_unique<types::BoolType>());
	types::IntType plain_int;

	EXPECT_TRUE(const_int.equals(&const_int_same));
	EXPECT_FALSE(const_int.equals(&const_bool));
	EXPECT_FALSE(const_int.equals(&plain_int));
}

TEST(Types, StaticType) {
	types::StaticType static_int(std::make_unique<types::IntType>());

	EXPECT_EQ(static_int.toString(), "static int");
	ASSERT_NE(static_int.getInnerType(), nullptr);
	EXPECT_EQ(static_int.getInnerType()->toString(), "int");

	types::StaticType static_int_same(std::make_unique<types::IntType>());
	types::StaticType static_bool(std::make_unique<types::BoolType>());
	types::IntType plain_int;

	EXPECT_TRUE(static_int.equals(&static_int_same));
	EXPECT_FALSE(static_int.equals(&static_bool));
	EXPECT_FALSE(static_int.equals(&plain_int));
}

TEST(Types, NestedQualifiers) {
	types::StaticType static_const_int(
	    std::make_unique<types::ConstType>(
		std::make_unique<types::IntType>()));

	EXPECT_EQ(static_const_int.toString(), "static const int");

	auto *inner = static_const_int.getInnerType();
	ASSERT_NE(inner, nullptr);
	auto *const_inner = dynamic_cast<types::ConstType *>(inner);
	ASSERT_NE(const_inner, nullptr);
	EXPECT_EQ(const_inner->getInnerType()->toString(), "int");
}
