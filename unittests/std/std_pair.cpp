//

#include <vide/types/std_pair.hpp>

#include <utility>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


TEST_CASE_TEMPLATE("std::pair", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	check_roundtrip<Ar>(random_value<std::pair<int, int>>(gen));
	for_each_sample_struct([&]<typename S> {
		check_roundtrip<Ar>(random_value<std::pair<S, S>>(gen));
		check_roundtrip<Ar>(random_value<std::pair<int, S>>(gen));
	});
}
