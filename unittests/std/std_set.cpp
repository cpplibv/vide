//

#include <vide/types/std_set.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::set", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic and string keys") {
		check_roundtrip<Ar>(random_container<std::set<std::string>>(gen, element_count));
		check_roundtrip<Ar>(random_container<std::set<std::int8_t>>(gen, element_count));
		check_roundtrip<Ar>(random_container<std::set<std::uint32_t>>(gen, element_count));
	}

	SUBCASE("struct keys") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::set<S>>(gen, element_count));
		});
	}

	SUBCASE("nested containers") {
		std::set<std::vector<StructInternalSerialize>> nested;
		for (std::size_t i = 0; i < 10; ++i)
			nested.insert(random_container<std::vector<StructInternalSerialize>>(gen, element_count));
		check_roundtrip<Ar>(nested);
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::set<int>{});
	}

	SUBCASE("out of order input") {
		// Same format as std::set, saved in descending order
		CHECK_EQ((roundtrip_as<Ar, std::set<int>>(
				std::set<int, std::greater<>>{3, 1, 4, 5, 9, 2, 6})),
				std::set<int>{1, 2, 3, 4, 5, 6, 9});
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = std::set<int>;
		using Multi = std::multiset<int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi{0, 1, 2})),
				Unique{0, 1, 2});
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi{0, 0, 1, 2})),
				vide::Exception);
	}
}
