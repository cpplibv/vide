//

#include <vide/types/std_unordered_set.hpp>

#include <cstddef>
#include <unordered_set>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

/// Every random element twice, so duplicates are present
template <typename C>
C random_with_duplicates(std::mt19937& gen) {
	C values;
	for (std::size_t i = 0; i < element_count; ++i) {
		const auto value = random_value<typename C::value_type>(gen);
		values.insert(value);
		values.insert(value);
	}
	return values;
}

TEST_CASE_TEMPLATE("std::unordered_multiset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicates") {
		check_roundtrip<Ar>(random_with_duplicates<std::unordered_multiset<int>>(gen));
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_with_duplicates<std::unordered_multiset<S, StructHash<S>>>(gen));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::unordered_multiset<int>{});
	}
}
