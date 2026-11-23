//

#include <vide/types/std_stack.hpp>

#include <cstddef>
#include <stack>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::stack", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_roundtrip<Ar>(random_container<std::stack<int>>(gen, element_count));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::stack<S>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::stack<int>{});
	}
}
