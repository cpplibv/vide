//

#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


TEST_CASE_TEMPLATE("struct serialization shapes", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("each shape") {
		for_each_sample_struct([&]<typename S> {
			const auto value = random_value<S>(gen);
			check_roundtrip<Ar>(value);
		});
	}

	SUBCASE("in sequence through one archive") {
		const auto a = random_value<StructInternalSerialize>(gen);
		const auto b = random_value<StructInternalSplit>(gen);
		const auto c = random_value<StructExternalSerialize>(gen);
		const auto d = random_value<StructExternalSplit>(gen);
		CHECK_EQ(roundtrip<Ar>(a, b, c, d), std::tuple{a, b, c, d});
	}
}
