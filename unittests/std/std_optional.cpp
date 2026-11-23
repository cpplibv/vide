//

#include <vide/types/std_optional.hpp>
#include <vide/types/std_string.hpp>

#include <optional>
#include <string>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


TEST_CASE_TEMPLATE("std::optional", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("engaged") {
		check_roundtrip<Ar>(std::optional<int>{random_value<int>(gen)});
		check_roundtrip<Ar>(std::optional<double>{random_value<double>(gen)});
		check_roundtrip<Ar>(std::optional<std::string>{random_value<std::string>(gen)});
		check_roundtrip<Ar>(std::optional<std::optional<long>>{random_value<long>(gen)});
		for_each_sample_struct([&]<typename S> {
			check_roundtrip<Ar>(std::optional<S>{random_value<S>(gen)});
		});
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::optional<int>{});
		check_roundtrip<Ar>(std::optional<std::string>{});
		check_roundtrip<Ar>(std::optional<std::optional<long>>{std::optional<long>{}});
		check_roundtrip<Ar>(std::optional<StructInternalSplit>{});
	}

	SUBCASE("loading empty resets an engaged value") {
		auto loaded = std::optional<int>{1};
		load_into<Ar>(save<Ar>(std::optional<int>{}), loaded);
		CHECK_FALSE(loaded.has_value());
	}
}
