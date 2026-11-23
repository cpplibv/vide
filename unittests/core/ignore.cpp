//

#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


namespace { // -------------------------------------------------------------------------------------

/// Two members are skipped on load (ignore<T>() and nvp_ignore<T>()) and recomputed from v0, so the values still
/// compare equal; the members after them only load right if the skip consumed exactly the saved data
struct Ignoring {
	int v0 = 0;
	int v1 = 0;
	int named = 0;
	int v2 = 0;
	int unnamed = 0;
	int v3 = 0;

	Ignoring() = default;
	explicit Ignoring(int value) :
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
		if constexpr (Archive::is_output) {
			ar.nvp("named", named);
		} else {
			ar.template nvp_ignore<int>("named");
			named = v0 + 2;
		}
		ar.nvp("v2", v2);
		if constexpr (Archive::is_output) {
			ar(unnamed);
		} else {
			ar.template ignore<int>();
			unnamed = v0 + 4;
		}
		ar.nvp("v3", v3);
	}

	bool operator==(const Ignoring&) const = default;
};

} // namespace -------------------------------------------------------------------------------------

TEST_CASE_TEMPLATE("ignore", Ar, VIDE_ALL_ARCHIVES) {
	check_roundtrip<Ar, Ignoring>(Ignoring{1});

	// In sequence, so an ignore must leave the archive where the next value starts
	const auto loaded = roundtrip<Ar>(Ignoring{10}, Ignoring{20}, Ignoring{30});
	CHECK_EQ(loaded, std::tuple{Ignoring{10}, Ignoring{20}, Ignoring{30}});
}
