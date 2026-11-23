//

#include <vide/types/memory.hpp>
#include <vide/types/std_tuple.hpp>

#include <memory>
#include <string>
#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


TEST_CASE_TEMPLATE("std::shared_ptr", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("shared pointees stay shared and are serialized once") {
		for_each_sample_struct([&]<typename S> {
			const auto x = std::make_shared<S>(random_value<S>(gen));
			const std::shared_ptr<const S> x_const = x;
			const auto y = std::make_shared<S>(random_value<S>(gen));
			const std::shared_ptr<S> null;

			const auto saved = std::tuple{x, x, x_const, y, y, null};
			const CallLog calls;
			const auto [x1, x2, x3, y1, y2, null1] = roundtrip<Ar>(saved);

			REQUIRE(x1);
			REQUIRE(y1);
			CHECK_EQ(*x1, *x);
			CHECK_EQ(*y1, *y);
			CHECK_EQ(x2, x1);
			CHECK_EQ(x3, x1);
			CHECK_EQ(y2, y1);
			CHECK_NE(x1, y1);
			CHECK_FALSE(null1);
			calls.check<S>(2);
		});
	}

	SUBCASE("arithmetic pointee") {
		const auto value = std::make_shared<int>(random_value<int>(gen));
		const auto [first, second] = roundtrip<Ar>(value, value);
		REQUIRE(first);
		CHECK_EQ(*first, *value);
		CHECK_EQ(second, first);
	}
}

TEST_CASE_TEMPLATE("std::unique_ptr", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	for_each_sample_struct([&]<typename S> {
		const auto value = std::make_unique<S>(random_value<S>(gen));
		const auto const_value = std::make_unique<const S>(random_value<S>(gen));
		const std::unique_ptr<S> null;

		const CallLog calls;
		const auto [loaded, loaded_const, loaded_null] = roundtrip<Ar>(value, const_value, null);

		REQUIRE(loaded);
		REQUIRE(loaded_const);
		CHECK_EQ(*loaded, *value);
		CHECK_EQ(*loaded_const, *const_value);
		CHECK_FALSE(loaded_null);
		calls.check<S>(2);
	});
}

/// Default constructor only reachable through vide::access
class PrivatelyConstructed {
	friend class vide::access;

	PrivatelyConstructed() = default;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
	}

public:
	int x = 0;

	explicit PrivatelyConstructed(int x) :
		x(x) {}
};

TEST_CASE_TEMPLATE("pointees are constructed through vide::access", Ar, VIDE_ALL_ARCHIVES) {
	const auto value = std::make_shared<PrivatelyConstructed>(1);
	const auto const_value = std::make_shared<const PrivatelyConstructed>(2);

	const CallLog calls;
	const auto [loaded, loaded_const] = roundtrip<Ar>(value, const_value);

	REQUIRE(loaded);
	REQUIRE(loaded_const);
	CHECK_EQ(loaded->x, 1);
	CHECK_EQ(loaded_const->x, 2);
	calls.check<PrivatelyConstructed>(2);
}

TEST_CASE_TEMPLATE("a shared pointee loaded as another type throws", Ar, VIDE_ALL_ARCHIVES) {
	const auto value = std::make_shared<int>(42);
	const std::string data = save<Ar>(std::tuple{value, value});
	using Mismatched = std::tuple<std::shared_ptr<int>, std::shared_ptr<double>>;
	CHECK_THROWS_AS((std::ignore = load<Ar, Mismatched>(data)), vide::Exception);
}
