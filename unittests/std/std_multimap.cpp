//

#include <vide/types/std_map.hpp>
#include <vide/types/std_string.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::multimap", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<std::multimap<std::string, int>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<std::multimap<std::int8_t, int>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<std::multimap<S, int>>(gen, element_count));
			check_roundtrip<Ar>(random_with_duplicates<std::multimap<std::int8_t, S>>(gen, element_count));
		});
	}

	SUBCASE("equal keys keep their order") { // the mapped value tells equal keys apart
		std::multimap<int, int> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}

	SUBCASE("out of order input") {
		CHECK_EQ((roundtrip_as<Ar, std::multimap<int, int>>(
				std::multimap<int, int, std::greater<>>{{3, 30}, {1, 10}, {4, 40}, {1, 11}, {2, 20}})),
				(std::multimap<int, int>{{1, 10}, {1, 11}, {2, 20}, {3, 30}, {4, 40}}));

		// Saved as (20, 1), (20, 2), (20, 3), (10, 1), ...; equal keys keep their order
		std::multimap<int, int, std::greater<>> descending;
		for (int value = 1; value <= 3; ++value) {
			descending.emplace(10, value);
			descending.emplace(20, value);
		}
		const auto loaded = roundtrip_as<Ar, std::multimap<int, int>>(descending);
		CHECK(std::ranges::equal(loaded, std::vector<std::pair<int, int>>{{10, 1}, {10, 2}, {10, 3}, {20, 1}, {20, 2}, {20, 3}}));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::multimap<int, int>{});
	}
}
