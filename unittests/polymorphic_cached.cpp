//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "polymorphic_test_classes.hpp"
#include "tester.hpp"

#include <iostream>


// =================================================================================================

template <class IArchive, class OArchive>
void test_polymorphic_cached() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	for (int i = 0; i < 100; ++i) {
		Tester<IArchive, OArchive> test;

		// --- Polymorphic loading via different types to the same object ---

		// Deep chain
		{
			struct _ {
				std::shared_ptr<DeepChain5> a;
				std::shared_ptr<DeepChain4> b;
				std::shared_ptr<DeepChain3> c;
				std::shared_ptr<DeepChain2> d;
				std::shared_ptr<DeepChain1> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		} {
			struct _ {
				std::shared_ptr<DeepChain1> a;
				std::shared_ptr<DeepChain2> b;
				std::shared_ptr<DeepChain3> c;
				std::shared_ptr<DeepChain4> d;
				std::shared_ptr<DeepChain5> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		} {
			struct _ {
				std::shared_ptr<DeepChain3> a;
				std::shared_ptr<DeepChain2> b;
				std::shared_ptr<DeepChain1> c;
				std::shared_ptr<DeepChain5> d;
				std::shared_ptr<DeepChain4> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		}

		{ // BaseAtNonZeroOffset
			struct _ {
				std::shared_ptr<BaseAtNonZeroOffset> a;
				std::shared_ptr<RootSimpleA> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			auto p = shared<BaseAtNonZeroOffset>(init);
			var.a = p;
			var.b = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<BaseAtNonZeroOffset>(in.a, out.a));
				CHECK_EQ(dcast<BaseAtNonZeroOffset>(in.a), dcast<BaseAtNonZeroOffset>(in.b));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<BaseAtNonZeroOffset> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			auto p = shared<BaseAtNonZeroOffset>(init);
			var.a = p;
			var.b = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<BaseAtNonZeroOffset>(in.a, out.a));
				CHECK_EQ(dcast<BaseAtNonZeroOffset>(in.a), dcast<BaseAtNonZeroOffset>(in.b));
			});
		}

		// ClassicVirtualDiamond
		{
			struct _ {
				std::shared_ptr<ClassicVirtualDiamond> a;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<RootSimpleA> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<ClassicVirtualDiamond> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<SingleVirtualBaseB_SimpleA> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<ClassicVirtualDiamond> c;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<SingleVirtualBaseA_SimpleA> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<ClassicVirtualDiamond> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		}

		{ // MultipleNonVirtualBases
			struct _ {
				std::shared_ptr<MultipleNonVirtualBases> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<RootSimpleB> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<MultipleNonVirtualBases> b;
				std::shared_ptr<RootSimpleB> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleB> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<MultipleNonVirtualBases> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
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
// 	test_polymorphic_cached<vide::JSONInputArchive, vide::JSONOutputArchive>();
// }
CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("polymorphic", test_polymorphic_cached)
CREATE_TEST_CASES_FOR_TEXT_ARCHIVE("polymorphic", test_polymorphic_cached)
// Proxy archives are skipped for test_polymorphic_cached as it takes ages to compile. A separate test will take care of it
// TODO P4: Enable for proxies too:
// CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic", test_polymorphic_cached)

TEST_SUITE_END();

