//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include <vide/types/reflection.hpp>


TEST_SUITE_BEGIN("types_reflection");


template<typename IArchive, typename OArchive>
struct Tester {
	template <typename T>
	bool operator()(const T& varOutput, const auto&... validators) {
		{ // Output with validators
			std::ostringstream os; {
				OArchive oar(os);
				oar(varOutput, validators...);
			}

			// if constexpr (IArchive::is_text_archive)
			// 	std::cout << os.str() << std::endl;
		}
		{ // Output without validators
			std::ostringstream os; {
				OArchive oar(os);
				oar(varOutput); // No validation on output so we can write out the invalid, so we can test the input validation
			}

			T varInput{};
			std::istringstream is(os.str()); {
				IArchive iar(is);
				iar(varInput, validators...);
			}

			return varOutput == varInput;
		}
	}
};

// -------------------------------------------------------------------------------------------------

struct is_odd {
	template <typename T>
	static inline void operator()(const T& var) {
		if (var % 2 == 0)
			throw vide::Exception("Is-odd validation failed during serialization: object is even.");
	}
};

// -------------------------------------------------------------------------------------------------

struct TestType0_empty {
	using serialize_enable_reflection = void;
	bool operator==(const TestType0_empty&) const = default;
};

struct TestType1 {
	using serialize_enable_reflection = void;
	int a = 1;
	bool operator==(const TestType1&) const = default;
};

struct TestType2 {
	using serialize_enable_reflection = void;
	int a = 2;
	int b = 3;
	int c = 4;
	bool operator==(const TestType2&) const = default;
};

struct TestType3_nested {
	using serialize_enable_reflection = void;
	TestType0_empty a;
	TestType1 b;
	TestType2 c;
	bool operator==(const TestType3_nested&) const = default;
};

struct TestType4_nested_versioned {
	static constexpr uint32_t serialize_class_version = 100;
	using serialize_enable_reflection = void;
	TestType0_empty a;
	TestType1 b;
	TestType2 c;
	bool operator==(const TestType4_nested_versioned&) const = default;
};

struct TestType5_validator {
	using serialize_enable_reflection = void;
	[[=vide::notnull]]
	[[=is_odd{}]]
	int a = 1;
	bool operator==(const TestType5_validator&) const = default;
};

struct TestType6_validator_with_unrelated {
	using serialize_enable_reflection = void;
	[[=3]] // Unrelated
	[[=vide::notnull]]
	// [[= [](int x) {
	// 	std::cout << "Validating: " << x << std::endl;
	// 	if (x == 1) throw vide::Exception{"Its one!"};
	// } ]]
	int a = 1;
	bool operator==(const TestType6_validator_with_unrelated&) const = default;
};

// -------------------------------------------------------------------------------------------------

template <typename IArchive, typename OArchive>
void test_types_reflection() {
	Tester<IArchive, OArchive> test{};

	CHECK(test(TestType0_empty{}));
	CHECK(test(TestType1{}));
	CHECK(test(TestType2{}));
	CHECK(test(TestType3_nested{}));

	CHECK(test(TestType4_nested_versioned{}));

	CHECK(test(TestType5_validator{1}));
	CHECK_THROWS_AS(test(TestType5_validator{0}), vide::Exception); // notnull fails
	CHECK_THROWS_AS(test(TestType5_validator{2}), vide::Exception); // is_odd fails

	CHECK(test(TestType6_validator_with_unrelated{2}));
	// CHECK_THROWS_AS(test(TestType6_validator_with_unrelated{1}), vide::Exception); // lambda fails
	CHECK_THROWS_AS(test(TestType6_validator_with_unrelated{0}), vide::Exception); // notnull fails
}


CREATE_TEST_CASES_FOR_ALL_ARCHIVE("types_reflection", test_types_reflection)

TEST_SUITE_END();
