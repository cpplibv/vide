//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "polymorphic_test_classes.hpp"
#include "tester.hpp"

#include <iostream>


// =================================================================================================

template <class IArchive, class OArchive>
void test_polymorphic_classes0() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	for (int i = 0; i < 100; ++i) {
		Tester<IArchive, OArchive> test;

		// --- RootRegularEmpty* ---

		test(shared<RootRegularEmptyA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});

		test(shared<RootRegularEmptyA>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularEmptyB>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularEmptyC>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootRegularSimple* ---

		test(shared<RootRegularSimpleA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});
		test(shared<RootRegularSimpleA>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularSimpleB>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularSimpleC>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootEmpty* ---

		test(shared<RootEmptyA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});

		test(shared<RootEmptyA>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootEmptyB>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootEmptyC>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootSimple* ---

		test(shared<RootSimpleA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});
		test(shared<RootSimpleA>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootSimpleB>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootSimpleC>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- LeafFinal_* ---

		test(shared<LeafFinal_EmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafFinal_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared_as<LeafFinal_EmptyA, RootEmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafFinal_EmptyA>(in, out));
		});

		test(shared_as<LeafFinal_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafFinal_SimpleA>(in, out));
		});

		// --- LeafNonFinal_* ---

		test(shared<LeafNonFinal_EmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared_as<LeafNonFinal_EmptyA, RootEmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinal_EmptyA>(in, out));
		});

		test(shared_as<LeafNonFinalA_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinalA_SimpleA>(in, out));
		});

		test(shared_as<LeafNonFinalB_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinalB_SimpleA>(in, out));
		});

		// --- BaseAtNonZeroOffset ---

		test(shared<BaseAtNonZeroOffset>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<BaseAtNonZeroOffset, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BaseAtNonZeroOffset>(in, out));
		});

		// --- MultipleNonVirtualBases ---

		test(shared<MultipleNonVirtualBases>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleNonVirtualBases, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualBases>(in, out));
		});
		test(shared_as<MultipleNonVirtualBases, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualBases>(in, out));
		});

		test(shared<MultipleNonVirtualReverseBases>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualReverseBases>(in, out));
		});
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualReverseBases>(in, out));
		});

		// --- SingleVirtualBase* ---

		test(shared<SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<SingleVirtualBaseA_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<SingleVirtualBaseA_SimpleA>(in, out));
		});

		test(shared<SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<SingleVirtualBaseB_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<SingleVirtualBaseB_SimpleA>(in, out));
		});

		// --- MultipleIndependentVirtualBase ---

		test(shared<MultipleIndependentVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleC>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});

		// --- *VirtualBaseAfter*VirtualBase ---

		test(shared<VirtualBaseAfterNonVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<VirtualBaseAfterNonVirtualBase>(in, out));
		});
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<VirtualBaseAfterNonVirtualBase>(in, out));
		});

		test(shared<NonVirtualBaseAfterVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<NonVirtualBaseAfterVirtualBase>(in, out));
		});
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<NonVirtualBaseAfterVirtualBase>(in, out));
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
// 	test_polymorphic_classes0<vide::JSONInputArchive, vide::JSONOutputArchive>();
// }
CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("polymorphic_classes", test_polymorphic_classes0)
CREATE_TEST_CASES_FOR_TEXT_ARCHIVE("polymorphic_classes", test_polymorphic_classes0)
// Proxy archives are skipped for test_polymorphic_classes0 as it takes ages to compile. A separate test will take care of it
// TODO P4: Enable for proxies too:
// CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic", test_polymorphic_classes0)

TEST_SUITE_END();

