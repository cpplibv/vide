//

#include <vide/types/std_bitset.hpp>

#include <bitset>
#include <cstddef>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"


template <std::size_t N>
std::bitset<N> random_bitset(std::mt19937& gen) {
	return std::bitset<N>(random_binary_string<N>(gen));
}

TEST_CASE_TEMPLATE("std::bitset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("sizes around the word boundaries") {
		check_roundtrip<Ar>(random_bitset<32>(gen));
		check_roundtrip<Ar>(random_bitset<65>(gen));
		check_roundtrip<Ar>(random_bitset<256>(gen));
		check_roundtrip<Ar>(random_bitset<512>(gen));
	}

	SUBCASE("all clear and all set") { // Loading starts from all clear, so all set shows a skipped load too
		check_roundtrip<Ar>(std::bitset<32>{});
		check_roundtrip<Ar>(std::bitset<32>{}.set());
		check_roundtrip<Ar>(std::bitset<65>{}.set());
	}
}
