//

#include <vide/types/std_string.hpp>
#include <vide/types/std_unordered_set.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_set>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::unordered_multiset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicate keys") {
		check_roundtrip<Ar>(random_with_duplicates<std::unordered_multiset<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_with_duplicates<std::unordered_multiset<std::int8_t>>(gen, element_count));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<std::unordered_multiset<S, StructHash<S>>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::unordered_multiset<int>{});
	}
}
