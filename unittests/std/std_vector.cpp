//

#include <vide/types/std_vector.hpp>

#include <cstddef>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::vector", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		const auto ints = random_container<std::vector<int>>(gen, element_count);
		check_roundtrip<Ar>(ints);
	}

	SUBCASE("bool") { // vector<bool> is bit packed, its own serializer
		const auto bools = random_container<std::vector<bool>>(gen, element_count);
		check_roundtrip<Ar>(bools);
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			const auto structs = random_container<std::vector<S>>(gen, element_count);
			check_roundtrip<Ar>(structs);
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::vector<int>{});
	}
}
