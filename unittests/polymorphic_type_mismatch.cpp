//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "common/my_intrusive_ptr_polymorphic.hpp"
#include "polymorphic_test_classes.hpp"

#include <iostream>


template <class IArchive, class OArchive>
inline void test_polymorphic_shared_ptr_type_mismatch() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	std::shared_ptr<RootSimpleA> o_ptr0 = std::make_shared<LeafNonFinalA_SimpleA>(init);
	std::shared_ptr<RootSimpleA> o_ptr1 = o_ptr0;
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

template <class IArchive, class OArchive>
inline void test_polymorphic_and_non_polymorphic_mismatch() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	std::shared_ptr<RootSimpleA> o_ptr0 = std::static_pointer_cast<RootSimpleA>(std::make_shared<LeafNonFinalA_SimpleA>(init));
	std::shared_ptr<RootSimpleA> o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<int> i_ptr1; //< This will not be the same type as the data was saved with

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
inline void test_polymorphic_and_non_polymorphic_mismatch_base() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	std::shared_ptr<RootSimpleA> o_ptr0 = std::make_shared<RootSimpleA>(init);
	std::shared_ptr<RootSimpleA> o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<int> i_ptr1; //< This will not be the same type as the data was saved with

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
inline void test_non_polymorphic_and_polymorphic_mismatch() {
	std::shared_ptr<int> o_ptr0 = std::make_shared<int>(32);
	std::shared_ptr<int> o_ptr1 = o_ptr0;
	std::shared_ptr<int> i_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr1; //< This will not be the same type as the data was saved with

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
inline void test_polymorphic_pointer_type_mismatch() {
	std::shared_ptr<TestRefCountedPolymorphic> o_ptr0 = std::make_shared<TestRefCountedPolymorphic>(32);
	std::shared_ptr<TestRefCountedPolymorphic> o_ptr1 = o_ptr0;
	std::shared_ptr<TestRefCountedPolymorphic> i_ptr0;
	my::intrusive_ptr<TestRefCountedPolymorphic> i_ptr1; //< This will not be the same type as the data was saved with

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
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_and_non_polymorphic_mismatch", test_polymorphic_and_non_polymorphic_mismatch)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_and_non_polymorphic_mismatch_base", test_polymorphic_and_non_polymorphic_mismatch_base)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("non_polymorphic_and_polymorphic_mismatch", test_non_polymorphic_and_polymorphic_mismatch)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_pointer_type_mismatch", test_polymorphic_pointer_type_mismatch)

TEST_SUITE_END();

