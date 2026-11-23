//

#include <vide/types/memory.hpp>
#include <vide/types/polymorphic.hpp>

#include <memory>
#include <random>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/random.hpp"
#include "common/shared_ptr_helpers.hpp"
#include "common/tester.hpp"
#include "test_classes.hpp"


// Proxy archives are left out: these tests take too long to compile for them (TODO P4)
TEST_CASE_TEMPLATE("polymorphic pointer cycles", Ar, VIDE_BINARY_ARCHIVES, VIDE_TEXT_ARCHIVES) {
	using IArchive = typename Ar::input;
	using OArchive = typename Ar::output;
	std::mt19937 rng = test_rng();
	Initializer init{rng};

	run_rounds<Ar>(100, rng, [&](Tester<Ar>& test) {
		// --- Self References ---

		{
			// A -> A
			const auto a = shared<WeakHolderBase>(init);
			a->ptrWeakHolderBase_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A base -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A derived -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderDerived_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A base -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Derived = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived: A derived -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Derived = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderDerived_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Base = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Base = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				const auto derivedIn = dcast<WeakHolderDerived>(in);
				CHECK((derivedIn && derivedIn->ptrWeakHolderDerived_Base.lock() == derivedIn));
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Derived = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				CHECK_EQ(in->ptrWeakHolderBase_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Derived = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				const auto derivedIn = dcast<WeakHolderDerived>(in);
				CHECK((derivedIn && derivedIn->ptrWeakHolderDerived_Derived.lock() == derivedIn));
			});
		}

		// --- Cyclic References ---

		{
			// A -> B
			// B -> A
			struct Holder1 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderBase>(init);
			var.b = shared<WeakHolderBase>(init);
			var.a->ptrWeakHolderBase_Base = var.b;
			var.b->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// A -> B
			// B -> C
			// C -> A
			struct Holder2 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderBase>(init);
			var.b = shared<WeakHolderBase>(init);
			var.c = shared<WeakHolderBase>(init);
			var.a->ptrWeakHolderBase_Base = var.b;
			var.b->ptrWeakHolderBase_Base = var.c;
			var.c->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.c);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(in.c->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B base
			// WeakHolderDerived as WeakHolderBase: B base -> A base
			struct Holder3 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Base = var.b;
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B base
			// WeakHolderDerived as WeakHolderBase: B base -> C base
			// WeakHolderDerived as WeakHolderBase: C base -> A base
			struct Holder4 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			var.c = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Base = var.b;
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Base = var.c;
			scast<WeakHolderDerived>(var.c)->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.c);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<WeakHolderDerived>(in.c, out.c));
				CHECK_EQ(in.c->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B derived
			// WeakHolderDerived as WeakHolderBase: B base -> A derived
			struct Holder5 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Derived.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Derived.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> B derived
			// WeakHolderDerived as WeakHolderBase: B derived -> A derived
			struct Holder6 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				const auto derivedInA = dcast<WeakHolderDerived>(in.a);
				const auto derivedInB = dcast<WeakHolderDerived>(in.b);
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK((derivedInA && derivedInA->ptrWeakHolderDerived_Derived.lock() == derivedInB));
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK((derivedInB && derivedInB->ptrWeakHolderDerived_Derived.lock() == derivedInA));
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> B derived
			// WeakHolderDerived as WeakHolderBase: B derived -> C derived
			// WeakHolderDerived as WeakHolderBase: C derived -> A derived
			struct Holder7 {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			var.c = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.c);
			scast<WeakHolderDerived>(var.c)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				const auto derivedInA = dcast<WeakHolderDerived>(in.a);
				const auto derivedInB = dcast<WeakHolderDerived>(in.b);
				const auto derivedInC = dcast<WeakHolderDerived>(in.c);
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK((derivedInA && derivedInA->ptrWeakHolderDerived_Derived.lock() == derivedInB));
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK((derivedInB && derivedInB->ptrWeakHolderDerived_Derived.lock() == derivedInC));
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<WeakHolderDerived>(in.c, out.c));
				CHECK((derivedInC && derivedInC->ptrWeakHolderDerived_Derived.lock() == derivedInA));
			});
		}
	});
}
