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
TEST_CASE_TEMPLATE("polymorphic class hierarchies", Ar, VIDE_BINARY_ARCHIVES, VIDE_TEXT_ARCHIVES) {
	std::mt19937 rng = test_rng();
	Initializer init{rng};

	run_rounds<Ar>(100, rng, [&](Tester<Ar>& test) {
		// --- RootRegularEmpty* ---

		test(shared<RootRegularEmptyA>(nullptr), expect_null);
		test(shared<RootRegularEmptyA>(), expect_equal_copy);
		test(shared<RootRegularEmptyB>(), expect_equal_copy);
		test(shared<RootRegularEmptyC>(), expect_equal_copy);

		// --- RootRegularSimple* ---

		test(shared<RootRegularSimpleA>(nullptr), expect_null);
		test(shared<RootRegularSimpleA>(init), expect_equal_copy);
		test(shared<RootRegularSimpleB>(init), expect_equal_copy);
		test(shared<RootRegularSimpleC>(init), expect_equal_copy);

		// --- RootEmpty* ---

		test(shared<RootEmptyA>(nullptr), expect_null);
		test(shared<RootEmptyA>(), expect_equal_copy);
		test(shared<RootEmptyB>(), expect_equal_copy);
		test(shared<RootEmptyC>(), expect_equal_copy);

		// --- RootSimple* ---

		test(shared<RootSimpleA>(nullptr), expect_null);
		test(shared<RootSimpleA>(init), expect_equal_copy);
		test(shared<RootSimpleB>(init), expect_equal_copy);
		test(shared<RootSimpleC>(init), expect_equal_copy);

		// --- LeafFinal_* ---

		test(shared<LeafFinal_EmptyA>(init), expect_equal);
		test(shared<LeafFinal_SimpleA>(init), expect_equal);
		test(shared_as<LeafFinal_EmptyA, RootEmptyA>(init), expect_equal_as<LeafFinal_EmptyA>);
		test(shared_as<LeafFinal_SimpleA, RootSimpleA>(init), expect_equal_as<LeafFinal_SimpleA>);

		// --- LeafNonFinal_* ---

		test(shared<LeafNonFinal_EmptyA>(init), expect_equal);
		test(shared<LeafNonFinalA_SimpleA>(init), expect_equal);
		test(shared<LeafNonFinalB_SimpleA>(init), expect_equal);
		test(shared_as<LeafNonFinal_EmptyA, RootEmptyA>(init), expect_equal_as<LeafNonFinal_EmptyA>);
		test(shared_as<LeafNonFinalA_SimpleA, RootSimpleA>(init), expect_equal_as<LeafNonFinalA_SimpleA>);
		test(shared_as<LeafNonFinalB_SimpleA, RootSimpleA>(init), expect_equal_as<LeafNonFinalB_SimpleA>);

		// --- BaseAtNonZeroOffset ---

		test(shared<BaseAtNonZeroOffset>(init), expect_equal);
		test(shared_as<BaseAtNonZeroOffset, RootSimpleA>(init), expect_equal_as<BaseAtNonZeroOffset>);

		// --- MultipleNonVirtualBases ---

		test(shared<MultipleNonVirtualBases>(init), expect_equal);
		test(shared_as<MultipleNonVirtualBases, RootSimpleA>(init), expect_equal_as<MultipleNonVirtualBases>);
		test(shared_as<MultipleNonVirtualBases, RootSimpleB>(init), expect_equal_as<MultipleNonVirtualBases>);
		test(shared<MultipleNonVirtualReverseBases>(init), expect_equal);
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleA>(init), expect_equal_as<MultipleNonVirtualReverseBases>);
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleB>(init), expect_equal_as<MultipleNonVirtualReverseBases>);

		// --- SingleVirtualBase* ---

		test(shared<SingleVirtualBaseA_SimpleA>(init), expect_equal);
		test(shared_as<SingleVirtualBaseA_SimpleA, RootSimpleA>(init), expect_equal_as<SingleVirtualBaseA_SimpleA>);
		test(shared<SingleVirtualBaseB_SimpleA>(init), expect_equal);
		test(shared_as<SingleVirtualBaseB_SimpleA, RootSimpleA>(init), expect_equal_as<SingleVirtualBaseB_SimpleA>);

		// --- MultipleIndependentVirtualBase ---

		test(shared<MultipleIndependentVirtualBase>(init), expect_equal);
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleA>(init), expect_equal_as<MultipleIndependentVirtualBase>);
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleB>(init), expect_equal_as<MultipleIndependentVirtualBase>);
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleC>(init), expect_equal_as<MultipleIndependentVirtualBase>);

		// --- *VirtualBaseAfter*VirtualBase ---

		test(shared<VirtualBaseAfterNonVirtualBase>(init), expect_equal);
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleA>(init), expect_equal_as<VirtualBaseAfterNonVirtualBase>);
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleB>(init), expect_equal_as<VirtualBaseAfterNonVirtualBase>);
		test(shared<NonVirtualBaseAfterVirtualBase>(init), expect_equal);
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleA>(init), expect_equal_as<NonVirtualBaseAfterVirtualBase>);
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleB>(init), expect_equal_as<NonVirtualBaseAfterVirtualBase>);
	});
}
