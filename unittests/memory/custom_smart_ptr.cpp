//

#include <vide/types/memory.hpp>
#include <vide/types/std_tuple.hpp>

#include <memory>
#include <string>
#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/my_intrusive_ptr.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"


TEST_CASE_TEMPLATE("custom smart pointer", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	const my::intrusive_ptr<TestRefCounted> x{new TestRefCounted{random_value<int>(gen)}};
	const my::intrusive_ptr<const TestRefCounted> x_const = x;
	const my::intrusive_ptr<TestRefCounted> y{new TestRefCounted{random_value<int>(gen)}};
	const my::intrusive_ptr<TestRefCounted> null;

	const auto saved = std::tuple{x, x, x_const, y, y, null};
	const CallLog calls;
	const auto [x1, x2, x3, y1, y2, null1] = roundtrip<Ar>(saved);

	REQUIRE(x1);
	REQUIRE(y1);
	CHECK_EQ(*x1, *x);
	CHECK_EQ(*y1, *y);
	CHECK_EQ(x2.get(), x1.get());
	CHECK_EQ(x3.get(), x1.get());
	CHECK_EQ(y2.get(), y1.get());
	CHECK_NE(x1.get(), y1.get());
	CHECK_FALSE(null1);
	calls.check<TestRefCounted>(2);
}

TEST_CASE_TEMPLATE("a custom smart pointee loaded through another pointer type throws", Ar, VIDE_ALL_ARCHIVES) {
	const my::intrusive_ptr<TestRefCounted> value{new TestRefCounted{22}};
	const std::string data = save<Ar>(std::tuple{value, value});
	using Mismatched = std::tuple<my::intrusive_ptr<TestRefCounted>, std::shared_ptr<TestRefCounted>>;
	CHECK_THROWS_AS((std::ignore = load<Ar, Mismatched>(data)), vide::Exception);
}
