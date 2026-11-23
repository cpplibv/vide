//

#include <vide/types/std_string.hpp>
#include <vide/types/std_unordered_map.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::unordered_multimap", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<std::unordered_multimap<std::string, int>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<std::unordered_multimap<std::int8_t, int>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<std::unordered_multimap<S, int, StructHash<S>>>(gen, element_count));
			check_roundtrip<Ar>(random_with_duplicates<std::unordered_multimap<std::int8_t, S>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::unordered_multimap<int, int>{});
	}
}
