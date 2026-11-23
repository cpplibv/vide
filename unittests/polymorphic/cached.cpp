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
TEST_CASE_TEMPLATE("polymorphic pointers with cached casts", Ar, VIDE_BINARY_ARCHIVES, VIDE_TEXT_ARCHIVES) {
	using IArchive = typename Ar::input;
	using OArchive = typename Ar::output;
	std::mt19937 rng = test_rng();
	Initializer init{rng};

	run_rounds<Ar>(100, rng, [&](Tester<Ar>& test) {
		// --- Polymorphic loading via different types to the same object ---

		// Deep chain
		{
			struct Holder1 {
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
			struct Holder2 {
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
			struct Holder3 {
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
			struct Holder4 {
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
			struct Holder5 {
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
			struct Holder6 {
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
			struct Holder7 {
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
			struct Holder8 {
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
			struct Holder9 {
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
			struct Holder10 {
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
			struct Holder11 {
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
			struct Holder12 {
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
	});
}
