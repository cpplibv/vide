//

#include <vide/types/boost_flat_set.hpp>
#include <vide/types/std_set.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <boost/container/flat_set.hpp>
#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/order_checker.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

template <typename K>
using flat_set = boost::container::flat_set<K>;

template <typename K>
using flat_multiset = boost::container::flat_multiset<K>;

// Expected values go through ordered_range / ordered_unique_range on purpose: brace initialization instantiates
// the bulk sort, a large compile time cost

TEST_CASE_TEMPLATE("boost::container::flat_set", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic and string keys") {
		check_roundtrip<Ar>(random_container<flat_set<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_container<flat_set<std::int8_t>>(gen, element_count));
	}

	SUBCASE("struct keys") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<flat_set<S>>(gen, element_count));
		});
	}

	SUBCASE("nested containers") {
		flat_set<std::vector<StructInternalSerialize>> nested;
		for (std::size_t i = 0; i < 10; ++i)
			nested.insert(random_container<std::vector<StructInternalSerialize>>(gen, element_count));
		check_roundtrip<Ar>(nested);
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(flat_set<int>{});
	}

	SUBCASE("out of order input") {
		// Same format as flat_set, saved in descending order
		CHECK_EQ((roundtrip_as<Ar, flat_set<int>>(
				std::set<int, std::greater<>>{3, 1, 4, 5, 9, 2, 6})),
				flat_set<int>(boost::container::ordered_unique_range, {1, 2, 3, 4, 5, 6, 9}));
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = flat_set<int>;
		using Multi = flat_multiset<int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi(boost::container::ordered_range, {0, 1, 2}))),
				Unique(boost::container::ordered_unique_range, {0, 1, 2}));
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi(boost::container::ordered_range, {0, 0, 1, 2}))),
				vide::Exception);
	}
}

TEST_CASE_TEMPLATE("boost::container::flat_multiset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<flat_multiset<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<flat_multiset<std::int8_t>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<flat_multiset<S>>(gen, element_count));
		});
	}

	SUBCASE("equal keys keep their order") {
		flat_multiset<OrderChecker> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}

	SUBCASE("out of order input") {
		CHECK_EQ((roundtrip_as<Ar, flat_multiset<int>>(
				std::multiset<int, std::greater<>>{3, 1, 4, 1, 5, 9, 2, 6, 5})),
				flat_multiset<int>(boost::container::ordered_range, {1, 1, 2, 3, 4, 5, 5, 6, 9}));

		// Saved as (20, 1), (20, 2), (20, 3), (10, 1), ...; equal keys keep their order
		std::multiset<OrderChecker, std::greater<>> descending;
		for (int value = 1; value <= 3; ++value) {
			descending.emplace(10, value);
			descending.emplace(20, value);
		}
		const auto loaded = roundtrip_as<Ar, flat_multiset<OrderChecker>>(descending);
		CHECK(std::ranges::equal(loaded, std::vector<OrderChecker>{{10, 1}, {10, 2}, {10, 3}, {20, 1}, {20, 2}, {20, 3}}));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(flat_multiset<int>{});
	}
}
