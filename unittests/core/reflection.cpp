//

#include <vide/types/reflection.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


struct is_odd {
	template <typename T>
	static void operator()(const T& var) {
		if (var % 2 == 0)
			throw vide::Exception("Is-odd validation failed during serialization: object is even.");
	}
};

// Values differ from the defaults, so a skipped load cannot pass as equal

struct Empty {
	using serialize_enable_reflection = void;
	bool operator==(const Empty&) const = default;
};

struct One {
	using serialize_enable_reflection = void;
	int a = 1;
	bool operator==(const One&) const = default;
};

struct Three {
	using serialize_enable_reflection = void;
	int a = 2;
	int b = 3;
	int c = 4;
	bool operator==(const Three&) const = default;
};

struct Nested {
	using serialize_enable_reflection = void;
	Empty a;
	One b;
	Three c;
	bool operator==(const Nested&) const = default;
};

struct NestedVersioned {
	static constexpr std::uint32_t serialize_class_version = 100;
	using serialize_enable_reflection = void;
	Empty a;
	One b;
	Three c;
	bool operator==(const NestedVersioned&) const = default;
};

/// Members with their own serialization functions; reflection must call each of them once
struct WithSampleStructs {
	using serialize_enable_reflection = void;
	StructInternalSerialize a;
	StructInternalSplit b;
	StructExternalSerialize c;
	StructExternalSplit d;
	bool operator==(const WithSampleStructs&) const = default;
};

struct Validated {
	using serialize_enable_reflection = void;
	[[=vide::notnull]]
	[[=is_odd{}]]
	int a = 1;
	bool operator==(const Validated&) const = default;
};

struct ValidatedWithUnrelatedAnnotation {
	using serialize_enable_reflection = void;
	[[=3]] // Not a validator, must be ignored
	[[=vide::notnull]]
	int a = 1;
	bool operator==(const ValidatedWithUnrelatedAnnotation&) const = default;
};

std::string without_whitespace(std::string text) {
	std::erase_if(text, [](unsigned char c) { return std::isspace(c); });
	return text;
}

TEST_CASE_TEMPLATE("reflection", Ar, VIDE_ALL_ARCHIVES) {
	SUBCASE("members") {
		check_roundtrip<Ar>(Empty{});
		check_roundtrip<Ar>(One{7});
		check_roundtrip<Ar>(Three{7, 8, 9});
		check_roundtrip<Ar>(Nested{{}, {7}, {8, 9, 10}});
		check_roundtrip<Ar>(NestedVersioned{{}, {7}, {8, 9, 10}});
	}

	SUBCASE("members with their own serialization functions") {
		check_roundtrip<Ar>(WithSampleStructs{{1, 2}, {3, 4}, {5, 6}, {7, 8}});
	}

	SUBCASE("annotated validators") {
		check_roundtrip<Ar>(Validated{7});
		CHECK_FALSE(saves<Ar>(Validated{0})); // notnull
		CHECK_FALSE(saves<Ar>(Validated{2})); // is_odd

		check_roundtrip<Ar>(ValidatedWithUnrelatedAnnotation{2});
		CHECK_FALSE(saves<Ar>(ValidatedWithUnrelatedAnnotation{0}));
	}
}

TEST_CASE("reflection names members after their identifiers") {
	CHECK_EQ(without_whitespace(save<JSONArchives>(Nested{{}, {7}, {8, 9, 10}})),
			R"({"value0":{"a":{},"b":{"a":7},"c":{"a":8,"b":9,"c":10}}})");
}
