//

#include <vide/types/memory.hpp>

#include <memory>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


struct SelfReferencing {
	int value = 0;
	std::weak_ptr<SelfReferencing> self;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(value)(self);
	}
};

TEST_CASE_TEMPLATE("a pointee referencing itself through a weak_ptr", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	const auto saved = std::make_shared<SelfReferencing>(random_value<int>(gen));
	saved->self = saved;

	const CallLog calls;
	const auto loaded = roundtrip<Ar>(saved);

	REQUIRE(loaded);
	CHECK_EQ(loaded->value, saved->value);
	CHECK_EQ(loaded->self.lock(), loaded);
	calls.check<SelfReferencing>(1);
}
