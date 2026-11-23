//

#include <vide/types/memory.hpp>
#include <vide/types/polymorphic.hpp>

#include <random>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/random.hpp"
#include "common/shared_ptr_helpers.hpp"
#include "common/tester.hpp"
#include "test_classes.hpp"


// Proxy archives are left out: these tests take too long to compile for them (TODO P4)
TEST_CASE_TEMPLATE("polymorphic class hierarchies through bases", Ar, VIDE_BINARY_ARCHIVES, VIDE_TEXT_ARCHIVES) {
	std::mt19937 rng = test_rng();
	Initializer init{rng};

	run_rounds<Ar>(100, rng, [&](Tester<Ar>& test) {
		// --- *VirtualDiamond ---

		test(shared<ClassicVirtualDiamond>(init), expect_equal);
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<ClassicVirtualDiamond>);
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), expect_equal_as<ClassicVirtualDiamond>);
		test(shared_as<ClassicVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<LeftVirtualVirtualDiamond>(init), expect_equal);
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<LeftVirtualVirtualDiamond>);
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), expect_equal_as<LeftVirtualVirtualDiamond>);
		test(shared_as<LeftVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<RightVirtualVirtualDiamond>(init), expect_equal);
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<RightVirtualVirtualDiamond>);
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), expect_equal_as<RightVirtualVirtualDiamond>);
		test(shared_as<RightVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<BothVirtualVirtualDiamond>(init), expect_equal);
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<BothVirtualVirtualDiamond>);
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), expect_equal_as<BothVirtualVirtualDiamond>);
		test(shared_as<BothVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		// --- MixedDepth ---

		test(shared<MixedDepth>(init), expect_equal);
		test(shared_as<MixedDepth, LeafNonFinalA_SimpleA>(init), expect_equal_as<MixedDepth>);
		test(shared_as<MixedDepth, RootSimpleA>(init), expect_equal_as<MixedDepth>);
		test(shared_as<MixedDepth, RootSimpleB>(init), expect_equal_as<MixedDepth>);

		// --- DuplicateNonVirtualBase ---

		test(shared<DuplicateNonVirtualBase>(init), expect_equal);
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init), expect_equal_as<DuplicateNonVirtualBase>);
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalB_SimpleA>(init), expect_equal_as<DuplicateNonVirtualBase>);
		test(scast<RootSimpleA>(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init)), expect_equal_as<DuplicateNonVirtualBase>);
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

		test(shared<DeepChain5>(init), expect_equal);
		test(shared_as<DeepChain5, DeepChain1>(init), expect_equal_as<DeepChain5>);
		test(shared_as<DeepChain5, DeepChain2>(init), expect_equal_as<DeepChain5>);
		test(shared_as<DeepChain5, DeepChain3>(init), expect_equal_as<DeepChain5>);
		test(shared_as<DeepChain5, DeepChain4>(init), expect_equal_as<DeepChain5>);

		// --- DeepVirtualChain5 ---

		test(shared<DeepVirtualChain5>(init), expect_equal);
		test(shared_as<DeepVirtualChain5, RootSimpleA>(init), expect_equal_as<DeepVirtualChain5>);
		test(shared_as<DeepVirtualChain5, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<DeepVirtualChain5>);
		test(shared_as<DeepVirtualChain5, DeepVirtualChain3>(init), expect_equal_as<DeepVirtualChain5>);
		test(shared_as<DeepVirtualChain5, DeepVirtualChain4>(init), expect_equal_as<DeepVirtualChain5>);

		// --- StressAggregate ---

		test(shared<StressAggregate>(init), expect_equal);
		test(shared_as<StressAggregate, LeafNonFinalA_SimpleA>(init), expect_equal_as<StressAggregate>);
		test(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init), expect_equal_as<StressAggregate>);
		test(shared_as<StressAggregate, LeafNonFinalB_SimpleA>(init), expect_equal_as<StressAggregate>);
		test(shared_as<StressAggregate, SingleVirtualBaseB_SimpleA>(init), expect_equal_as<StressAggregate>);
		test(scast<RootSimpleA>(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init)), [](const auto& out, const auto& in) {
			// BUG: DuplicateNonVirtualBase downcast incorrectly (see DuplicateNonVirtualBase)
			// CHECK_EQ(*in, *out); This should pass
			CHECK_EQ(*scast<RootSimpleA>(dcast<SingleVirtualBaseA_SimpleA>(in)), *out); // Current status quo
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
	});
}
