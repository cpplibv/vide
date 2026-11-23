//

#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


namespace { // -------------------------------------------------------------------------------------

/// Two members are loaded by return value (load<T>() and nvp_load<T>()) instead of into a reference
struct DirectLoaded {
	int v0 = 0;
	int v1 = 0;
	int named = 0;
	int v2 = 0;
	int unnamed = 0;
	int v3 = 0;

	DirectLoaded() = default;
	explicit DirectLoaded(int value) :
		v0(value),
		v1(value + 1),
		named(value + 2),
		v2(value + 3),
		unnamed(value + 4),
		v3(value + 5) {}

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar.nvp("v0", v0);
		ar.nvp("v1", v1);
		if constexpr (Archive::is_output)
			ar.nvp("named", named);
		else
			named = ar.template nvp_load<int>("named");
		ar.nvp("v2", v2);
		if constexpr (Archive::is_output)
			ar(unnamed);
		else
			unnamed = ar.template load<int>();
		ar.nvp("v3", v3);
	}

	bool operator==(const DirectLoaded&) const = default;
};

} // namespace -------------------------------------------------------------------------------------

TEST_CASE_TEMPLATE("direct load", Ar, VIDE_ALL_ARCHIVES) {
	check_roundtrip<Ar, DirectLoaded>(DirectLoaded{1});

	// In sequence, so a direct load must leave the archive where the next value starts
	const auto loaded = roundtrip<Ar>(DirectLoaded{10}, DirectLoaded{20}, DirectLoaded{30});
	CHECK_EQ(loaded, std::tuple{DirectLoaded{10}, DirectLoaded{20}, DirectLoaded{30}});
}
