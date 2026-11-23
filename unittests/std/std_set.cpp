//

#include <vide/types/std_set.hpp>

#include <cstddef>
#include <set>
#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::set", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_roundtrip<Ar>(random_container<std::set<int>>(gen, element_count));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::set<S>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::set<int>{});
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = std::set<int>;
		using Multi = std::multiset<int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi{0, 1, 2})),
				Unique{0, 1, 2});
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi{0, 0, 1, 2})),
				vide::Exception);
	}
}
