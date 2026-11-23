//

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/roundtrip.hpp"


namespace { // ------------------------------------------------------------------------------------------

static_assert(vide::enum_value_set::is_unbounded<std::byte>);
static_assert(vide::enum_value_set::is_binary_serializable<std::byte>);

// --- test_nospec -----------------------------------------------------------------------------------------------------

enum class test_nospec_unbounded_0 : int32_t {
};
static_assert(0 == vide::enum_value_set::count_specifiers<test_nospec_unbounded_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_nospec_unbounded_0>);

enum class test_nospec_unbounded_1 : int32_t {
	a,
	b,
	c,
};
static_assert(0 == vide::enum_value_set::count_specifiers<test_nospec_unbounded_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_nospec_unbounded_1>);

enum class test_nospec_unbounded_2 : int32_t {
	a,
	b,
	c,
	count,
};
static_assert(0 == vide::enum_value_set::count_specifiers<test_nospec_unbounded_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_nospec_unbounded_2>);

enum class test_nospec_unbounded_3 : int32_t {
	count,
};
static_assert(0 == vide::enum_value_set::count_specifiers<test_nospec_unbounded_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_nospec_unbounded_3>);

// --- test_enumerator_unbounded ---------------------------------------------------------------------------------------

enum class test_enumerator_unbounded_0 : int32_t {
	serialize_unbounded,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_unbounded_0>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_enumerator_unbounded_0>);
static_assert(vide::enum_value_set::is_unbounded<test_enumerator_unbounded_0>);

enum class test_enumerator_unbounded_1 : int32_t {
	a,
	b,
	c,
	serialize_unbounded,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_unbounded_1>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_enumerator_unbounded_1>);
static_assert(vide::enum_value_set::is_unbounded<test_enumerator_unbounded_1>);

enum class test_enumerator_unbounded_2 : int32_t {
	a,
	b,
	c,
	count,
	serialize_unbounded, // value doest not matter, but could be set to = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_unbounded_2>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_enumerator_unbounded_2>);
static_assert(vide::enum_value_set::is_unbounded<test_enumerator_unbounded_2>);

enum class test_enumerator_unbounded_3 : int32_t {
	count,
	serialize_unbounded, // value doest not matter, but could be set to = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_unbounded_3>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_enumerator_unbounded_3>);
static_assert(vide::enum_value_set::is_unbounded<test_enumerator_unbounded_3>);

// --- test_enumerator_end_value ---------------------------------------------------------------------------------------

enum class test_enumerator_end_value_0 : int32_t {
	serialize_end_value,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_0>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_0>);

enum class test_enumerator_end_value_1 : int32_t {
	a,
	b,
	c,
	serialize_end_value,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_1>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_1>);

enum class test_enumerator_end_value_2 : int32_t {
	a,
	b,
	c,
	count,
	serialize_end_value = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_2>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_2>);

enum class test_enumerator_end_value_3 : int32_t {
	count,
	serialize_end_value = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_3>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_3>);

// --- test_enumerator_end_value_unsigned ------------------------------------------------------------------------------

enum class test_enumerator_end_value_unsigned_0 : uint32_t {
	serialize_end_value,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_unsigned_0>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_unsigned_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_unsigned_0>);

enum class test_enumerator_end_value_unsigned_1 : uint32_t {
	a,
	b,
	c,
	serialize_end_value,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_unsigned_1>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_unsigned_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_unsigned_1>);

enum class test_enumerator_end_value_unsigned_2 : uint32_t {
	a,
	b,
	c,
	count,
	serialize_end_value = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_unsigned_2>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_unsigned_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_unsigned_2>);

enum class test_enumerator_end_value_unsigned_3 : uint32_t {
	count,
	serialize_end_value = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_end_value_3>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_enumerator_end_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_end_value_3>);

// --- test_enumerator_max_value ---------------------------------------------------------------------------------------

enum class test_enumerator_max_value_0 : int32_t {
	serialize_max_value,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_0>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_0>);

enum class test_enumerator_max_value_1 : int32_t {
	a,
	b,
	c,
	serialize_max_value = c,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_1>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_1>);

enum class test_enumerator_max_value_2 : int32_t {
	a,
	b,
	c,
	count,
	serialize_max_value = c,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_2>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_2>);

enum class test_enumerator_max_value_3 : int32_t {
	count,
	serialize_max_value = count,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_3>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_3>);

// --- test_enumerator_max_value_unsigned ------------------------------------------------------------------------------

enum class test_enumerator_max_value_unsigned_0 : uint32_t {
	serialize_max_value = 0, // max_value cannot specify empty enum value set
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_unsigned_0>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_unsigned_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_unsigned_0>);

enum class test_enumerator_max_value_unsigned_1 : uint32_t {
	a,
	b,
	c,
	serialize_max_value = c,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_unsigned_1>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_unsigned_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_unsigned_1>);

enum class test_enumerator_max_value_unsigned_2 : uint32_t {
	a,
	b,
	c,
	count,
	serialize_max_value = c,
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_unsigned_2>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_unsigned_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_unsigned_2>);

enum class test_enumerator_max_value_unsigned_3 : uint32_t {
	count,
	serialize_max_value = 0, // max_value cannot specify empty enum value set
};
static_assert(1 == vide::enum_value_set::count_specifiers<test_enumerator_max_value_3>);
static_assert(vide::enum_value_set::has_max_value_enumerator<test_enumerator_max_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_enumerator_max_value_3>);

// --- test_free_function_unbounded ------------------------------------------------------------------------------------

enum class test_free_function_unbounded_0 : int32_t {
};
void serialize_enum_unbounded(test_free_function_unbounded_0) {}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_unbounded_0>);
static_assert(vide::enum_value_set::has_unbounded_free_function<test_free_function_unbounded_0>);
static_assert(vide::enum_value_set::is_unbounded<test_free_function_unbounded_0>);

enum class test_free_function_unbounded_1 : int32_t {
	a,
	b,
	c,
};
void serialize_enum_unbounded(test_free_function_unbounded_1) {}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_unbounded_1>);
static_assert(vide::enum_value_set::has_unbounded_free_function<test_free_function_unbounded_1>);
static_assert(vide::enum_value_set::is_unbounded<test_free_function_unbounded_1>);

enum class test_free_function_unbounded_2 : int32_t {
	a,
	b,
	c,
	count,
};
void serialize_enum_unbounded(test_free_function_unbounded_2) {}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_unbounded_2>);
static_assert(vide::enum_value_set::has_unbounded_free_function<test_free_function_unbounded_2>);
static_assert(vide::enum_value_set::is_unbounded<test_free_function_unbounded_2>);

enum class test_free_function_unbounded_3 : int32_t {
	count,
};
void serialize_enum_unbounded(test_free_function_unbounded_3) {}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_unbounded_3>);
static_assert(vide::enum_value_set::has_unbounded_free_function<test_free_function_unbounded_3>);
static_assert(vide::enum_value_set::is_unbounded<test_free_function_unbounded_3>);

// --- test_free_function_end_value ------------------------------------------------------------------------------------

enum class test_free_function_end_value_0 : int32_t {
};
test_free_function_end_value_0 serialize_enum_end_value(test_free_function_end_value_0) {
	return test_free_function_end_value_0{0};
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_end_value_0>);
static_assert(vide::enum_value_set::has_end_value_free_function_enum<test_free_function_end_value_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_end_value_0>);

enum class test_free_function_end_value_1 : int32_t {
	a,
	b,
	c,
};
std::underlying_type_t<test_free_function_end_value_1> serialize_enum_end_value(test_free_function_end_value_1) {
	return std::to_underlying(test_free_function_end_value_1::c) + 1;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_end_value_1>);
static_assert(vide::enum_value_set::has_end_value_free_function_underlying<test_free_function_end_value_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_end_value_1>);

enum class test_free_function_end_value_2 : int32_t {
	a,
	b,
	c,
	count,
};
test_free_function_end_value_2 serialize_enum_end_value(test_free_function_end_value_2) {
	return test_free_function_end_value_2::count;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_end_value_2>);
static_assert(vide::enum_value_set::has_end_value_free_function_enum<test_free_function_end_value_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_end_value_2>);

enum class test_free_function_end_value_3 : int32_t {
	count,
};
std::underlying_type_t<test_free_function_end_value_3> serialize_enum_end_value(test_free_function_end_value_3) {
	return std::to_underlying(test_free_function_end_value_3::count);
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_end_value_3>);
static_assert(vide::enum_value_set::has_end_value_free_function_underlying<test_free_function_end_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_end_value_3>);

// --- test_free_function_max_value ------------------------------------------------------------------------------------

enum class test_free_function_max_value_0 : int32_t {
};
test_free_function_max_value_0 serialize_enum_max_value(test_free_function_max_value_0) {
	return test_free_function_max_value_0{0}; // max_value cannot specify empty enum value set
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_max_value_0>);
static_assert(vide::enum_value_set::has_max_value_free_function_enum<test_free_function_max_value_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_max_value_0>);

enum class test_free_function_max_value_1 : int32_t {
	a,
	b,
	c,
};
std::underlying_type_t<test_free_function_max_value_1> serialize_enum_max_value(test_free_function_max_value_1) {
	return std::to_underlying(test_free_function_max_value_1::c);
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_max_value_1>);
static_assert(vide::enum_value_set::has_max_value_free_function_underlying<test_free_function_max_value_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_max_value_1>);

enum class test_free_function_max_value_2 : int32_t {
	a,
	b,
	c,
	count,
};
test_free_function_max_value_2 serialize_enum_max_value(test_free_function_max_value_2) {
	return test_free_function_max_value_2::c;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_max_value_2>);
static_assert(vide::enum_value_set::has_max_value_free_function_enum<test_free_function_max_value_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_max_value_2>);

enum class test_free_function_max_value_3 : int32_t {
	count,
};
std::underlying_type_t<test_free_function_max_value_3> serialize_enum_max_value(test_free_function_max_value_3) {
	return std::to_underlying(test_free_function_max_value_3::count); // max_value cannot specify empty enum value set
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_max_value_3>);
static_assert(vide::enum_value_set::has_max_value_free_function_underlying<test_free_function_max_value_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_max_value_3>);

// --- test_free_function_verify ---------------------------------------------------------------------------------------

enum class test_free_function_verify_0 : int32_t {
};
bool serialize_enum_verify(test_free_function_verify_0) {
	return false;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_verify_0>);
static_assert(vide::enum_value_set::has_verify_free_function<test_free_function_verify_0>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_verify_0>);

enum class test_free_function_verify_1 : int32_t {
	a,
	b,
	c,
};
bool serialize_enum_verify(test_free_function_verify_1 var) {
	switch (var) {
	case test_free_function_verify_1::a: [[fallthrough]];
	case test_free_function_verify_1::b: [[fallthrough]];
	case test_free_function_verify_1::c:
		return true;
	default:
		return false;
	}
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_verify_1>);
static_assert(vide::enum_value_set::has_verify_free_function<test_free_function_verify_1>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_verify_1>);

enum class test_free_function_verify_2 : int32_t {
	a,
	b,
	c,
	count,
};
bool serialize_enum_verify(test_free_function_verify_2 var) {
	switch (var) {
	case test_free_function_verify_2::a: [[fallthrough]];
	case test_free_function_verify_2::b: [[fallthrough]];
	case test_free_function_verify_2::c:
		return true;
	default:
		return false;
	}
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_verify_2>);
static_assert(vide::enum_value_set::has_verify_free_function<test_free_function_verify_2>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_verify_2>);

enum class test_free_function_verify_3 : int32_t {
	count,
};
bool serialize_enum_verify(test_free_function_verify_3) {
	return false;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_verify_3>);
static_assert(vide::enum_value_set::has_verify_free_function<test_free_function_verify_3>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_verify_3>);

enum class test_free_function_verify_odd_only : int32_t {
};
bool serialize_enum_verify(test_free_function_verify_odd_only var) {
	return std::to_underlying(var) % 2 != 0;
}
static_assert(1 == vide::enum_value_set::count_specifiers<test_free_function_verify_odd_only>);
static_assert(vide::enum_value_set::has_verify_free_function<test_free_function_verify_odd_only>);
static_assert(!vide::enum_value_set::is_unbounded<test_free_function_verify_odd_only>);

// --- test_incorrect_multispec ----------------------------------------------------------------------------------------

enum class test_incorrect_multispec_0 : int32_t {
	serialize_unbounded,
	serialize_end_value,
};
static_assert(2 == vide::enum_value_set::count_specifiers<test_incorrect_multispec_0>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_unbounded_free_function<test_incorrect_multispec_0>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_end_value_free_function_enum<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_end_value_free_function_underlying<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_max_value_enumerator<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_max_value_free_function_enum<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_max_value_free_function_underlying<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_verify_free_function<test_incorrect_multispec_0>);
static_assert(vide::enum_value_set::is_unbounded<test_incorrect_multispec_0>);

enum class test_incorrect_multispec_1 : int32_t {
	serialize_unbounded,
};
void serialize_enum_unbounded(test_incorrect_multispec_1) {}
static_assert(2 == vide::enum_value_set::count_specifiers<test_incorrect_multispec_1>);
static_assert(vide::enum_value_set::has_unbounded_enumerator<test_incorrect_multispec_1>);
static_assert(vide::enum_value_set::has_unbounded_free_function<test_incorrect_multispec_1>);
static_assert(vide::enum_value_set::has_end_value_enumerator<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_end_value_free_function_enum<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_end_value_free_function_underlying<test_incorrect_multispec_0>);
static_assert(!vide::enum_value_set::has_max_value_enumerator<test_incorrect_multispec_1>);
static_assert(!vide::enum_value_set::has_max_value_free_function_enum<test_incorrect_multispec_1>);
static_assert(!vide::enum_value_set::has_max_value_free_function_underlying<test_incorrect_multispec_1>);
static_assert(!vide::enum_value_set::has_verify_free_function<test_incorrect_multispec_1>);
static_assert(vide::enum_value_set::is_unbounded<test_incorrect_multispec_1>);

} // namespace ------------------------------------------------------------------------------------------

/// enum_value_set::verify() accepts each value; values are enumerators or underlying values
template <typename E>
void verify_accepts(auto... values) {
	([&] {
		const E value = static_cast<E>(values);
		CAPTURE(std::to_underlying(value));
		CHECK_NOTHROW(vide::enum_value_set::verify(value));
	}(), ...);
}

/// enum_value_set::verify() rejects each value
template <typename E>
void verify_rejects(auto... values) {
	([&] {
		const E value = static_cast<E>(values);
		CAPTURE(std::to_underlying(value));
		CHECK_THROWS_AS(vide::enum_value_set::verify(value), vide::Exception);
	}(), ...);
}

TEST_CASE("enum value sets") {
	verify_accepts<test_nospec_unbounded_0>(0, 42, -42);
	verify_accepts<test_nospec_unbounded_1>(0, 42, -42);
	verify_accepts<test_nospec_unbounded_2>(test_nospec_unbounded_2::a, test_nospec_unbounded_2::b, test_nospec_unbounded_2::c, 0, 42, -42);
	verify_accepts<test_nospec_unbounded_3>(0, 42, -42);

	verify_accepts<test_enumerator_unbounded_0>(0, 42, -42);
	verify_accepts<test_enumerator_unbounded_1>(0, 42, -42);
	verify_accepts<test_enumerator_unbounded_2>(test_enumerator_unbounded_2::a, test_enumerator_unbounded_2::b, test_enumerator_unbounded_2::c, 0, 42, -42);
	verify_accepts<test_enumerator_unbounded_3>(0, 42, -42);

	verify_accepts<test_free_function_unbounded_0>(0, 42, -42);
	verify_accepts<test_free_function_unbounded_1>(0, 42, -42);
	verify_accepts<test_free_function_unbounded_2>(test_free_function_unbounded_2::a, test_free_function_unbounded_2::b, test_free_function_unbounded_2::c, 0, 42, -42);
	verify_accepts<test_free_function_unbounded_3>(0, 42, -42);

	verify_rejects<test_enumerator_end_value_0>(-1, 0, 1, 42, -42);
	verify_accepts<test_enumerator_end_value_1>(0, 1, 2);
	verify_rejects<test_enumerator_end_value_1>(-1, 3, 42, -42);
	verify_accepts<test_enumerator_end_value_2>(test_enumerator_end_value_2::a, test_enumerator_end_value_2::b, test_enumerator_end_value_2::c, 0, 1, 2);
	verify_rejects<test_enumerator_end_value_2>(-1, 3, 42, -42);
	verify_rejects<test_enumerator_end_value_3>(0, 42, -42);

	verify_rejects<test_enumerator_end_value_unsigned_0>(0, 1, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));
	verify_accepts<test_enumerator_end_value_unsigned_1>(0, 1, 2);
	verify_rejects<test_enumerator_end_value_unsigned_1>(3, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));
	verify_accepts<test_enumerator_end_value_unsigned_2>(test_enumerator_end_value_unsigned_2::a, test_enumerator_end_value_unsigned_2::b, test_enumerator_end_value_unsigned_2::c, 0, 1, 2);
	verify_rejects<test_enumerator_end_value_unsigned_2>(3, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));
	verify_rejects<test_enumerator_end_value_unsigned_3>(0, 1, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));

	verify_accepts<test_enumerator_max_value_0>(0);
	verify_rejects<test_enumerator_max_value_0>(-1, 1, 42, -42);  // max_value cannot specify empty enum value set
	verify_accepts<test_enumerator_max_value_1>(0, 1, 2);
	verify_rejects<test_enumerator_max_value_1>(-1, 3, 42, -42);
	verify_accepts<test_enumerator_max_value_2>(test_enumerator_max_value_2::a, test_enumerator_max_value_2::b, test_enumerator_max_value_2::c, 0, 1, 2);
	verify_rejects<test_enumerator_max_value_2>(-1, 3, 42, -42);
	verify_accepts<test_enumerator_max_value_3>(0);
	verify_rejects<test_enumerator_max_value_3>(42, -42);  // max_value cannot specify empty enum value set

	verify_accepts<test_enumerator_max_value_unsigned_0>(0);
	verify_rejects<test_enumerator_max_value_unsigned_0>(1, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));  // max_value cannot specify empty enum value set
	verify_accepts<test_enumerator_max_value_unsigned_1>(0, 1, 2);
	verify_rejects<test_enumerator_max_value_unsigned_1>(3, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));
	verify_accepts<test_enumerator_max_value_unsigned_2>(test_enumerator_max_value_unsigned_2::a, test_enumerator_max_value_unsigned_2::b, test_enumerator_max_value_unsigned_2::c, 0, 1, 2);
	verify_rejects<test_enumerator_max_value_unsigned_2>(3, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));
	verify_accepts<test_enumerator_max_value_unsigned_3>(0);
	verify_rejects<test_enumerator_max_value_unsigned_3>(1, 42, static_cast<uint32_t>(-1), static_cast<uint32_t>(-42));  // max_value cannot specify empty enum value set

	verify_rejects<test_free_function_end_value_0>(0, 42, -42);
	verify_accepts<test_free_function_end_value_1>(0);
	verify_rejects<test_free_function_end_value_1>(42, -42);
	verify_accepts<test_free_function_end_value_2>(test_free_function_end_value_2::a, test_free_function_end_value_2::b, test_free_function_end_value_2::c, 0);
	verify_rejects<test_free_function_end_value_2>(42, -42);
	verify_rejects<test_free_function_end_value_3>(0, 42, -42);

	verify_accepts<test_free_function_max_value_0>(0);
	verify_rejects<test_free_function_max_value_0>(42, -42);
	verify_accepts<test_free_function_max_value_1>(0);
	verify_rejects<test_free_function_max_value_1>(42, -42);
	verify_accepts<test_free_function_max_value_2>(test_free_function_max_value_2::a, test_free_function_max_value_2::b, test_free_function_max_value_2::c, 0);
	verify_rejects<test_free_function_max_value_2>(42, -42);
	verify_accepts<test_free_function_max_value_3>(0);
	verify_rejects<test_free_function_max_value_3>(42, -42);

	verify_rejects<test_free_function_verify_0>(0, 42, -42);
	verify_accepts<test_free_function_verify_1>(0);
	verify_rejects<test_free_function_verify_1>(42, -42);
	verify_accepts<test_free_function_verify_2>(test_free_function_verify_2::a, test_free_function_verify_2::b, test_free_function_verify_2::c, 0);
	verify_rejects<test_free_function_verify_2>(42, -42);
	verify_rejects<test_free_function_verify_3>(0, 42, -42);

	verify_accepts<test_free_function_verify_odd_only>(-1, 1, 3);
	verify_rejects<test_free_function_verify_odd_only>(0, 2, 42, -42);

	// test_incorrect_multispec_* declare two value sets; verify() on them must not compile
}

TEST_CASE_TEMPLATE("enum serialization", Ar, VIDE_ALL_ARCHIVES) {
	enum class Bounded : std::uint32_t {
		a,
		b,
		c,
		serialize_end_value,
	};

	enum class Unbounded : std::uint32_t {
		a,
		b,
		c,
		serialize_unbounded,
	};

	SUBCASE("values in the set") { // A loaded enum starts as 0 (a), so b and c show a load happened
		check_roundtrip<Ar>(Bounded::a);
		check_roundtrip<Ar>(Bounded::b);
		check_roundtrip<Ar>(Bounded::c);
		check_roundtrip<Ar>(Unbounded{3});
	}

	SUBCASE("a value outside the set throws on save") {
		CHECK_FALSE(saves<Ar>(Bounded{3}));
	}

	SUBCASE("a value outside the set throws on load") {
		CHECK_THROWS_AS((std::ignore = load<Ar, Bounded>(save<Ar>(Unbounded{3}))), vide::Exception);
	}
}
