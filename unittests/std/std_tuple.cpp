//

#include <vide/types/std_tuple.hpp>

#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


template <typename... Ts>
std::tuple<Ts...> random_tuple(std::mt19937& gen) {
	return std::tuple<Ts...>{random_value<Ts>(gen)...};
}

TEST_CASE_TEMPLATE("std::tuple", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_roundtrip<Ar>(std::tuple<>{});
		check_roundtrip<Ar>(random_tuple<int>(gen));
		check_roundtrip<Ar>(random_tuple<int, double, char, long long>(gen));
		check_roundtrip<Ar>(random_tuple<int, int, int, int, int, int, int, int, int, int, int>(gen));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_tuple<S, S, S, S>(gen));
		});
		check_roundtrip<Ar>(random_tuple<StructInternalSerialize, StructInternalSplit, StructExternalSerialize, StructExternalSplit>(gen));
	}
}
