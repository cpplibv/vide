//

#include <vide/types/polymorphic.hpp>
#include <vide/types/std_tuple.hpp>

#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/my_intrusive_ptr_polymorphic.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


struct TestRefCountedPolymorphicDerived : TestRefCountedPolymorphic {
	using TestRefCountedPolymorphic::TestRefCountedPolymorphic;

	template <typename Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar.template base_class<TestRefCountedPolymorphic>(this);
	}
};

VIDE_REGISTER_TYPE(TestRefCountedPolymorphicDerived)

using Base = TestRefCountedPolymorphic;
using Derived = TestRefCountedPolymorphicDerived;

template <typename T>
using ptr = my::intrusive_ptr<T>;

TEST_CASE_TEMPLATE("polymorphic custom smart pointer", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();
	const auto random_base = [&] { return ptr<Base>{new Base{random_value<int>(gen)}}; };
	const auto random_derived = [&] { return ptr<Derived>{new Derived{random_value<int>(gen)}}; };

	// Every object is referenced twice, through the pointer types in the comment
	const ptr<Base> x = random_base();                        // Base, Base, const Base
	const ptr<Base> y = random_base();                        // Base, Base
	const ptr<Base> a = random_derived();                     // Base, Base
	const ptr<Derived> b = random_derived();                  // Derived, Derived
	const ptr<Derived> c = random_derived();                  // Derived, Base
	const ptr<Base> d = random_derived();                     // Base, Derived
	const ptr<Derived> d_derived{static_cast<Derived*>(d.get())};
	const ptr<Base> null;

	const auto saved = std::tuple{x, x, ptr<const Base>{x}, y, y, a, a, b, b, c, ptr<Base>{c}, d, d_derived, null, null};
	const CallLog calls;
	const auto [x1, x2, x3, y1, y2, a1, a2, b1, b2, c1, c2, d1, d2, null1, null2] = roundtrip<Ar>(saved);

	CHECK_EQ(*x1, *x);
	CHECK_EQ(*y1, *y);
	CHECK_EQ(*a1, *a);
	CHECK_EQ(*b1, *b);
	CHECK_EQ(*c1, *c);
	CHECK_EQ(*d1, *d);
	CHECK(dynamic_cast<const Derived*>(a1.get()));
	CHECK(dynamic_cast<const Derived*>(d1.get()));

	// Shared objects stay shared
	CHECK_EQ(x2.get(), x1.get());
	CHECK_EQ(x3.get(), x1.get());
	CHECK_EQ(y2.get(), y1.get());
	CHECK_EQ(a2.get(), a1.get());
	CHECK_EQ(b2.get(), b1.get());
	CHECK_EQ(c2.get(), c1.get());
	CHECK_EQ(d2.get(), d1.get());
	CHECK_FALSE(null1);
	CHECK_FALSE(null2);

	// Six objects, each serialized once; the four derived ones reach Base through base_class()
	calls.check<Derived>(4);
	calls.check<Base>(6);
}
