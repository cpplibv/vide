//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "polymorphic_test_classes.hpp"

#include <iostream>


template <class IArchive, class OArchive>
inline void test_polymorphic_shared_ptr_type_mismatch() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	auto o_ptr0 = std::static_pointer_cast<RootSimpleA>(std::make_shared<LeafNonFinalA_SimpleA>(init));
	auto o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<RootSimpleB> i_ptr1; //< This will not be the same type as the data was saved with

	std::ostringstream os; {
		OArchive oar(os);
		oar(o_ptr0);
		oar(o_ptr1);
	} {
		std::istringstream is(os.str());
		IArchive iar(is);
		iar(i_ptr0);
		CHECK_THROWS_AS(iar(i_ptr1), vide::Exception);
	}
}

template <class IArchive, class OArchive>
inline void test_polymorphic_shared_ptr_type_mismatch_base() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	auto o_ptr0 = std::make_shared<RootSimpleA>(init);
	auto o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<RootSimpleB> i_ptr1; //< This will not be the same type as the data was saved with

	std::ostringstream os; {
		OArchive oar(os);
		oar(o_ptr0);
		oar(o_ptr1);
	} {
		std::istringstream is(os.str());
		IArchive iar(is);
		iar(i_ptr0);
		CHECK_THROWS_AS(iar(i_ptr1), vide::Exception);
	}
}


TEST_SUITE_BEGIN("polymorphic");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_shared_ptr_type_mismatch", test_polymorphic_shared_ptr_type_mismatch)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_shared_ptr_type_mismatch_base", test_polymorphic_shared_ptr_type_mismatch_base)

TEST_SUITE_END();

