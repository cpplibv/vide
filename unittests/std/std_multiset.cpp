//

#include <vide/types/std_set.hpp>
#include <vide/types/std_string.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/order_checker.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::multiset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<std::multiset<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<std::multiset<std::int8_t>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<std::multiset<S>>(gen, element_count));
		});
	}

	SUBCASE("equal keys keep their order") {
		std::multiset<OrderChecker> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}

	SUBCASE("out of order input") {
		CHECK_EQ((roundtrip_as<Ar, std::multiset<int>>(
				std::multiset<int, std::greater<>>{3, 1, 4, 1, 5, 9, 2, 6, 5})),
				std::multiset<int>{1, 1, 2, 3, 4, 5, 5, 6, 9});

		// Saved as (20, 1), (20, 2), (20, 3), (10, 1), ...; equal keys keep their order
		std::multiset<OrderChecker, std::greater<>> descending;
		for (int value = 1; value <= 3; ++value) {
			descending.emplace(10, value);
			descending.emplace(20, value);
		}
		const auto loaded = roundtrip_as<Ar, std::multiset<OrderChecker>>(descending);
		CHECK(std::ranges::equal(loaded, std::vector<OrderChecker>{{10, 1}, {10, 2}, {10, 3}, {20, 1}, {20, 2}, {20, 3}}));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::multiset<int>{});
	}
}
