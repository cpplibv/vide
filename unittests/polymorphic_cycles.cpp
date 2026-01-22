//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "polymorphic_test_classes.hpp"
#include "tester.hpp"

#include <iostream>


// =================================================================================================

template <class IArchive, class OArchive>
void test_polymorphic_cycles() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	for (int i = 0; i < 100; ++i) {
		Tester<IArchive, OArchive> test;

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
			struct _ {
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
			struct _ {
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
			struct _ {
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
			struct _ {
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
			struct _ {
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
			struct _ {
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
			struct _ {
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

		// === Runner code ===

		if (i != 0 && i != 99) // First and last is not shuffled for ease of debugging
			test.shuffle(rng);

		std::ostringstream os; {
			OArchive oar(os);
			test.outputAll(oar);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		std::istringstream is(os.str()); {
			IArchive iar(is);
			test.inputAll(iar);
		}

		test.checkAll();
	}
}


TEST_SUITE_BEGIN("polymorphic");

// TEST_CASE("json_polymorphic") {
// 	test_polymorphic_cycles<vide::JSONInputArchive, vide::JSONOutputArchive>();
// }
CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("polymorphic", test_polymorphic_cycles)
CREATE_TEST_CASES_FOR_TEXT_ARCHIVE("polymorphic", test_polymorphic_cycles)
// Proxy archives are skipped for test_polymorphic_cycles as it takes ages to compile. A separate test will take care of it
// TODO P4: Enable for proxies too:
// CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic", test_polymorphic_cycles)

TEST_SUITE_END();

