//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


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

TEST_SUITE_BEGIN("enum");

TEST_CASE("test_enum_common") {
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_0{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_0{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_0{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_1{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_1{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_2{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_3{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_3{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_nospec_unbounded_3{-42}));

	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_0{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_0{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_0{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_1{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_1{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_2{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_3{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_3{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_unbounded_3{-42}));

	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_0{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_0{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_0{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_1{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_1{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_2{-42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_3{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_3{42}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_unbounded_3{-42}));

	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_0{-1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_0{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_0{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_0{-42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_1{-1}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_1{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_1{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_1{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_1{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2::c));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_2{-1}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_2{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_2{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_2{-42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_3{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_3{-42}), vide::Exception);

	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_0{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_0{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_0{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_0{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_1{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_2{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_3{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_3{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_3{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_end_value_unsigned_3{static_cast<uint32_t>(-42)}), vide::Exception);

	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_0{-1}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_0{0})); // max_value cannot specify empty enum value set
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_0{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_0{-42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_1{-1}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_1{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_1{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_1{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_1{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2::c));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_2{-1}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_2{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_2{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_2{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_3{0})); // max_value cannot specify empty enum value set
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_3{-42}), vide::Exception);

	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_0{0})); // max_value cannot specify empty enum value set
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_0{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_0{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_0{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_1{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{0}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{1}));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{2}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{3}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_2{static_cast<uint32_t>(-42)}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_3{0})); // max_value cannot specify empty enum value set
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_3{1}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_3{static_cast<uint32_t>(-1)}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_enumerator_max_value_unsigned_3{static_cast<uint32_t>(-42)}), vide::Exception);

	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_0{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_0{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_end_value_1{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_1{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_end_value_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_end_value_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_end_value_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_end_value_2{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_2{-42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_3{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_end_value_3{-42}), vide::Exception);

	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_0{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_0{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_1{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_1{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_2{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_2{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_max_value_3{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_max_value_3{-42}), vide::Exception);

	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_0{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_0{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_0{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_1{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_1{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_1{-42}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_2::a));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_2::b));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_2::c));
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_2{0}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_2{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_2{-42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_3{0}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_3{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_3{-42}), vide::Exception);

	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_odd_only{-1}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_odd_only{0}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_odd_only{1}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_odd_only{2}), vide::Exception);
	CHECK_NOTHROW(vide::enum_value_set::verify(test_free_function_verify_odd_only{3}));
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_odd_only{42}), vide::Exception);
	CHECK_THROWS_AS(vide::enum_value_set::verify(test_free_function_verify_odd_only{-42}), vide::Exception);

	// CHECK_NOTHROW(vide::enum_value_set::verify(test_incorrect_multispec_0{0})); // Correctly fails to compile
	// CHECK_NOTHROW(vide::enum_value_set::verify(test_incorrect_multispec_1{0})); // Correctly fails to compile
}

template <typename IArchive, typename OArchive>
void test_enum() {
	enum class enum0 : uint32_t {
		a,
		b,
		c,
		serialize_end_value,
	};

	enum class enumUnbounded0 : uint32_t {
		a,
		b,
		c,
		serialize_unbounded,
	};

	auto output0 = enum0::a;
	auto output1 = enum0::b;
	auto output2 = enum0::c;
	auto output3 = enum0{3};
	auto outputEnumUnbounded3 = enumUnbounded0{3};

	std::ostringstream osWillFail; {
		OArchive oar(osWillFail);
		oar(output0);
		oar(output1);
		oar(output2);
		CHECK_THROWS_AS(oar(output3), vide::Exception);
	}

	std::ostringstream os; {
		OArchive oar(os);
		oar(output0);
		oar(output1);
		oar(output2);
		oar(outputEnumUnbounded3);
	}

	enum0 input0;
	enum0 input1;
	enum0 input2;
	enum0 input3;
	enumUnbounded0 inputEnumUnbounded3;

	std::istringstream isWillFail(os.str()); {
		IArchive iar(isWillFail);

		iar(input0);
		iar(input1);
		iar(input2);
		CHECK_THROWS_AS(iar(input3), vide::Exception);
	}

	std::istringstream is(os.str()); {
		IArchive iar(is);

		iar(input0);
		iar(input1);
		iar(input2);
		iar(inputEnumUnbounded3);
	}

	CHECK_EQ(input0, output0);
	CHECK_EQ(input1, output1);
	CHECK_EQ(input2, output2);
	// CHECK_EQ(input3, output3);
	CHECK_EQ(inputEnumUnbounded3, outputEnumUnbounded3);
}

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("enum", test_enum)

TEST_SUITE_END();
