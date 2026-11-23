//

#include <vide/types/std_array.hpp>

#include <algorithm>
#include <array>
#include <cstddef>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


template <typename T, std::size_t N>
std::array<T, N> random_array(std::mt19937& gen) {
	std::array<T, N> values;
	std::ranges::generate(values, [&] { return random_value<T>(gen); });
	return values;
}

TEST_CASE_TEMPLATE("std::array", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_roundtrip<Ar>(random_array<int, 100>(gen));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_array<S, 100>(gen));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::array<int, 0>{});
		check_roundtrip<Ar>(std::array<StructInternalSerialize, 0>{});
	}
}
