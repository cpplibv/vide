//

#include <vide/types/boost_flat_set.hpp>
#include <vide/types/std_set.hpp>

#include <cstddef>
#include <set>
#include <tuple>

#include <boost/container/flat_set.hpp>
#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("boost::container::flat_set", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_roundtrip<Ar>(random_container<boost::container::flat_set<int>>(gen, element_count));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<boost::container::flat_set<S>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(boost::container::flat_set<int>{});
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		// vide has no flat_multiset serializer; std::multiset writes the same format
		using Unique = boost::container::flat_set<int>;
		using Multi = std::multiset<int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi{0, 1, 2})),
				Unique{0, 1, 2});
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi{0, 0, 1, 2})),
				vide::Exception);
	}
}
