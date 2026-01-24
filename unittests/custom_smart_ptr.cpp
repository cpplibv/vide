//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "common/my_intrusive_ptr.hpp"


template <class IArchive, class OArchive>
inline void test_custom_smart_ptr() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		my::intrusive_ptr<TestRefCounted> o_xptr1 = my::intrusive_ptr<TestRefCounted>(new TestRefCounted{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCounted> o_xptr2 = o_xptr1;
		my::intrusive_ptr<const TestRefCounted> o_xptr3 = o_xptr1;
		my::intrusive_ptr<TestRefCounted> o_yptr1 = my::intrusive_ptr<TestRefCounted>(new TestRefCounted{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCounted> o_yptr2 = o_yptr1;
		my::intrusive_ptr<TestRefCounted> o_nullptr1;
		my::intrusive_ptr<TestRefCounted> o_nullptr2;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_xptr1);
			oar(o_xptr2);
			oar(o_xptr3);
			oar(o_yptr1);
			oar(o_yptr2);
			oar(o_nullptr1);
			oar(o_nullptr2);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		my::intrusive_ptr<TestRefCounted> i_xptr1;
		my::intrusive_ptr<TestRefCounted> i_xptr2;
		my::intrusive_ptr<const TestRefCounted> i_xptr3;
		my::intrusive_ptr<TestRefCounted> i_yptr1;
		my::intrusive_ptr<TestRefCounted> i_yptr2;
		my::intrusive_ptr<TestRefCounted> i_nullptr1;
		my::intrusive_ptr<TestRefCounted> i_nullptr2;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_xptr1);
			iar(i_xptr2);
			iar(i_xptr3);
			iar(i_yptr1);
			iar(i_yptr2);
			iar(i_nullptr1);
			iar(i_nullptr2);
		}

		CHECK_EQ(o_xptr1.get(), o_xptr2.get());
		CHECK_EQ(o_xptr1.get(), o_xptr3.get());
		CHECK_EQ(o_xptr1.get(), o_xptr1.get());

		CHECK_EQ(o_yptr1.get(), o_yptr2.get());

		CHECK_EQ(i_xptr1.get(), i_xptr2.get());
		CHECK_EQ(i_xptr1.get(), i_xptr3.get());
		CHECK_EQ(i_yptr1.get(), i_yptr2.get());

		CHECK_EQ(*i_xptr1, *o_xptr1);
		CHECK_EQ(*i_xptr2, *o_xptr2);
		CHECK_EQ(*i_xptr3, *o_xptr3);
		CHECK_EQ(*i_yptr1, *o_yptr1);
		CHECK_EQ(*i_yptr2, *o_yptr2);
		CHECK_UNARY_FALSE(i_nullptr1);
		CHECK_UNARY_FALSE(i_nullptr2);
	}
}

template <class IArchive, class OArchive>
inline void test_custom_smart_ptr_pointer_type_mismatch() {
	my::intrusive_ptr<TestRefCounted> o_xptr1 = my::intrusive_ptr<TestRefCounted>(new TestRefCounted{22});
	my::intrusive_ptr<TestRefCounted> o_xptr2 = o_xptr1;

	std::ostringstream os; {
		OArchive oar(os);

		oar(o_xptr1);
		oar(o_xptr2);
	}

	my::intrusive_ptr<TestRefCounted> i_xptr1;
	std::shared_ptr<TestRefCounted> i_xptr2; //< This will not be the same type as the data was saved with

	std::istringstream is(os.str()); {
		IArchive iar(is);

		iar(i_xptr1);
		CHECK_THROWS_AS(iar(i_xptr2), vide::Exception);
	}
}


TEST_SUITE_BEGIN("custom_smart_ptr");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("custom_smart_ptr", test_custom_smart_ptr)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("custom_smart_ptr_pointer_type_mismatch", test_custom_smart_ptr_pointer_type_mismatch)

TEST_SUITE_END();
