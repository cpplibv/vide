//

#include <vide/types/std_map.hpp>
#include <vide/types/std_string.hpp>

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

/// Every random key twice, with different values
template <typename C>
C random_with_duplicate_keys(std::mt19937& gen) {
	C values;
	for (std::size_t i = 0; i < element_count; ++i) {
		const auto key = random_value<typename C::key_type>(gen);
		values.emplace(key, random_value<typename C::mapped_type>(gen));
		values.emplace(key, random_value<typename C::mapped_type>(gen));
	}
	return values;
}

TEST_CASE_TEMPLATE("std::multimap", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicate_keys<std::multimap<std::string, int>>(gen));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicate_keys<std::multimap<std::int8_t, S>>(gen));
		});
	}

	SUBCASE("equal keys keep their order") { // multimap == compares in order, equal keys included
		std::multimap<int, int> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::multimap<int, int>{});
	}
}
