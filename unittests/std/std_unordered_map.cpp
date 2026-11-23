//

#include <vide/types/std_string.hpp>
#include <vide/types/std_unordered_map.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

TEST_CASE_TEMPLATE("std::unordered_map", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic and string keys") {
		check_roundtrip<Ar>(random_container<std::unordered_map<std::string, int>>(gen, element_count));
		check_roundtrip<Ar>(random_container<std::unordered_map<std::int8_t, int>>(gen, element_count));
	}

	SUBCASE("struct values") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::unordered_map<int, S>>(gen, element_count));
		});
	}

	SUBCASE("struct keys") {
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(random_container<std::unordered_map<S, int, StructHash<S>>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::unordered_map<int, int>{});
	}

	SUBCASE("duplicate keys") {
		// Using the multi-container counterpart to force duplicate keys into the archive
		using Unique = std::unordered_map<int, int>;
		using Multi = std::unordered_multimap<int, int>;
		CHECK_EQ((roundtrip_as<Ar, Unique>(
				Multi{{0, 42}, {1, 42}, {2, 42}})),
				Unique{{0, 42}, {1, 42}, {2, 42}});
		CHECK_THROWS_AS((std::ignore = roundtrip_as<Ar, Unique>(
				Multi{{0, 42}, {1, 42}, {1, 43}, {2, 42}})),
				vide::Exception);
	}
}
