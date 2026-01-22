//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "polymorphic_test_classes.hpp"
#include "tester.hpp"

#include <iostream>


// =================================================================================================

template <class IArchive, class OArchive>
void test_polymorphic_classes1() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	for (int i = 0; i < 100; ++i) {
		Tester<IArchive, OArchive> test;

		// --- *VirtualDiamond ---

		test(shared<ClassicVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
		});
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
		});
		test(shared_as<ClassicVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<LeftVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<LeftVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<RightVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<RightVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<BothVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<BothVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		// --- MixedDepth ---

		test(shared<MixedDepth>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MixedDepth, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});
		test(shared_as<MixedDepth, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});
		test(shared_as<MixedDepth, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});

		// --- DuplicateNonVirtualBase ---

		test(shared<DuplicateNonVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init)), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<DuplicateNonVirtualBase, LeafNonFinalB_SimpleA>(init)), [](const auto& out, const auto& in) {
			// BUG: DuplicateNonVirtualBase downcast incorrectly selects the first child instead of what was pointed to
			// So this is a funny setup, and definitely a bug. When we save pointing to RootSimpleA #2 and loading
			// it back with RootSimpleA incorrectly RootSimple #1 will be selected.
			// To fix this we need have to detect duplicate types in the hierarchy and also serialize an index during
			// downcast and use that index during upcast.
			//
			//           Input ->  RootSimple #1    RootSimpleA #2  <- Output
			//                           ^                ^
			//                           |                |
			//                     LeafNonFinalA    LeafNonFinalB
			//                            ^              ^
			//                             \            /
			//                        DuplicateNonVirtualBase

			// CHECK_EQ(*in, *out); This should pass
			CHECK_EQ(*scast<RootSimpleA>(dcast<LeafNonFinalB_SimpleA>(in)), *out); // Current status quo
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});

		// --- DeepChain5 ---

		test(shared<DeepChain5>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DeepChain5, DeepChain1>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain2>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain3>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain4>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});

		// --- DeepVirtualChain5 ---

		test(shared<DeepVirtualChain5>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DeepVirtualChain5, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, DeepVirtualChain3>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, DeepVirtualChain4>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});

		// --- StressAggregate ---

		test(shared<StressAggregate>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<StressAggregate, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init)), [](const auto& out, const auto& in) {
			// BUG: DuplicateNonVirtualBase downcast incorrectly (see DuplicateNonVirtualBase)
			// CHECK_EQ(*in, *out); This should pass
			CHECK_EQ(*scast<RootSimpleA>(dcast<SingleVirtualBaseA_SimpleA>(in)), *out); // Current status quo
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});

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
// 	test_polymorphic_classes1<vide::JSONInputArchive, vide::JSONOutputArchive>();
// }
CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("polymorphic_classes", test_polymorphic_classes1)
CREATE_TEST_CASES_FOR_TEXT_ARCHIVE("polymorphic_classes", test_polymorphic_classes1)
// Proxy archives are skipped for test_polymorphic_classes1 as it takes ages to compile. A separate test will take care of it
// TODO P4: Enable for proxies too:
// CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic", test_polymorphic_classes1)

TEST_SUITE_END();

