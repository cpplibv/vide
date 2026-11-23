//

#include <vide/types/std_string.hpp>
#include <vide/types/std_variant.hpp>

#include <string>
#include <variant>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


TEST_CASE_TEMPLATE("std::variant", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	using Variant = std::variant<int, double, std::string, StructInternalSplit, StructExternalSerialize>;

	// Each alternative; the default constructed variant holds int, so the others also show the index is restored
	check_roundtrip<Ar>(Variant{random_value<int>(gen)});
	check_roundtrip<Ar>(Variant{random_value<double>(gen)});
	check_roundtrip<Ar>(Variant{random_value<std::string>(gen)});
	check_roundtrip<Ar>(Variant{random_value<StructInternalSplit>(gen)});
	check_roundtrip<Ar>(Variant{random_value<StructExternalSerialize>(gen)});
	check_roundtrip<Ar>(std::variant<std::monostate, int>{});
}
