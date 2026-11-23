//

#include <vide/types/std_string.hpp>
#include <vide/types/std_unordered_set.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_set>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::unordered_set", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic and string keys") {
		check_roundtrip<Ar>(random_container<std::unordered_set<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_container<std::unordered_set<std::int8_t>>(gen, element_count));
	}

	SUBCASE("struct keys") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::unordered_set<S, StructHash<S>>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::unordered_set<int>{});
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = std::unordered_set<int>;
		using Multi = std::unordered_multiset<int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi{0, 1, 2})),
				Unique{0, 1, 2});
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi{0, 0, 1, 2})),
				vide::Exception);
	}
}
