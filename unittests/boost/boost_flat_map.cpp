//

#include <vide/types/boost_flat_map.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/std_map.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/container/flat_map.hpp>
#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

template <typename K, typename V>
using flat_map = boost::container::flat_map<K, V>;

template <typename K, typename V>
using flat_multimap = boost::container::flat_multimap<K, V>;

// Expected values go through ordered_range / ordered_unique_range on purpose: brace initialization instantiates
// the bulk sort, a large compile time cost

TEST_CASE_TEMPLATE("boost::container::flat_map", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic and string keys") {
		check_roundtrip<Ar>(random_container<flat_map<std::string, int>>(gen, element_count));
		check_roundtrip<Ar>(random_container<flat_map<std::int8_t, int>>(gen, element_count));
	}

	SUBCASE("struct keys") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<flat_map<S, int>>(gen, element_count));
		});
	}

	SUBCASE("struct values") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<flat_map<int, S>>(gen, element_count));
		});
	}

	SUBCASE("nested containers") {
		flat_map<std::size_t, std::vector<StructInternalSerialize>> nested;
		for (std::size_t i = 0; i < 10; ++i)
			nested[random_value<std::size_t>(gen)] = random_container<std::vector<StructInternalSerialize>>(gen, element_count);
		check_roundtrip<Ar>(nested);
	}

	SUBCASE("smart pointer values") {
		flat_map<int, std::unique_ptr<int>> unique;
		flat_map<int, std::shared_ptr<int>> shared;
		for (std::size_t i = 0; i < element_count; ++i) {
			unique.emplace(random_value<int>(gen), std::make_unique<int>(random_value<int>(gen)));
			shared.emplace(random_value<int>(gen), std::make_shared<int>(random_value<int>(gen)));
		}

		const auto [loaded_unique, loaded_shared] = roundtrip<Ar>(unique, shared);
		const auto same_pointees = [](const auto& a, const auto& b) {
			return std::ranges::equal(a, b, [](const auto& x, const auto& y) { return x.first == y.first && *x.second == *y.second; });
		};
		CHECK(same_pointees(loaded_unique, unique));
		CHECK(same_pointees(loaded_shared, shared));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(flat_map<int, int>{});
	}

	SUBCASE("out of order input") {
		// Same format as flat_map, saved in descending key order
		CHECK_EQ((roundtrip_as<Ar, flat_map<int, int>>(
				std::map<int, int, std::greater<>>{{3, 30}, {1, 10}, {4, 40}, {2, 20}})),
				flat_map<int, int>(boost::container::ordered_unique_range, {{1, 10}, {2, 20}, {3, 30}, {4, 40}}));
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = flat_map<int, int>;
		using Multi = flat_multimap<int, int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi(boost::container::ordered_range, {{0, 42}, {1, 42}, {2, 42}}))),
				Unique(boost::container::ordered_unique_range, {{0, 42}, {1, 42}, {2, 42}}));
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi(boost::container::ordered_range, {{0, 42}, {1, 42}, {1, 43}, {2, 42}}))),
				vide::Exception);
	}
}

TEST_CASE_TEMPLATE("boost::container::flat_multimap", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<flat_multimap<std::string, int>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<flat_multimap<std::int8_t, int>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<flat_multimap<S, int>>(gen, element_count));
			check_roundtrip<Ar>(random_with_duplicates<flat_multimap<std::int8_t, S>>(gen, element_count));
		});
	}

	SUBCASE("equal keys keep their order") { // the mapped value tells equal keys apart
		flat_multimap<int, int> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}

	SUBCASE("out of order input") {
		CHECK_EQ((roundtrip_as<Ar, flat_multimap<int, int>>(
				std::multimap<int, int, std::greater<>>{{3, 30}, {1, 10}, {4, 40}, {1, 11}, {2, 20}})),
				flat_multimap<int, int>(boost::container::ordered_range, {{1, 10}, {1, 11}, {2, 20}, {3, 30}, {4, 40}}));

		// Saved as (20, 1), (20, 2), (20, 3), (10, 1), ...; equal keys keep their order
		std::multimap<int, int, std::greater<>> descending;
		for (int value = 1; value <= 3; ++value) {
			descending.emplace(10, value);
			descending.emplace(20, value);
		}
		const auto loaded = roundtrip_as<Ar, flat_multimap<int, int>>(descending);
		CHECK(std::ranges::equal(loaded, std::vector<std::pair<int, int>>{{10, 1}, {10, 2}, {10, 3}, {20, 1}, {20, 2}, {20, 3}}));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(flat_multimap<int, int>{});
	}
}
