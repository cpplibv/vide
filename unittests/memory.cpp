//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
inline void test_memory() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::shared_ptr<int> o_xptr1 = std::make_shared<int>(random_value<int>(gen));
		std::shared_ptr<int> o_xptr2 = o_xptr1;
		std::shared_ptr<const int> o_xptr3 = o_xptr1;
		std::shared_ptr<int> o_yptr1 = std::make_shared<int>(random_value<int>(gen));
		std::shared_ptr<int> o_yptr2 = o_yptr1;
		std::shared_ptr<int> o_nullptr1;
		std::shared_ptr<int> o_nullptr2;

		std::unique_ptr<int> o_zptr1(new int(random_value<int>(gen)));
		std::unique_ptr<const int> o_zptr2(new int(random_value<int>(gen)));
		std::unique_ptr<int> o_nullptr3;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_xptr1)(o_xptr2)(o_xptr3);
			oar(o_yptr1)(o_yptr2);
			oar(o_nullptr1)(o_nullptr2);

			oar(o_zptr1)(o_zptr2);
			oar(o_nullptr3);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		std::shared_ptr<int> i_xptr1;
		std::shared_ptr<int> i_xptr2;
		std::shared_ptr<const int> i_xptr3;
		std::shared_ptr<int> i_yptr1;
		std::shared_ptr<int> i_yptr2;
		std::shared_ptr<int> i_nullptr1;
		std::shared_ptr<int> i_nullptr2;

		std::unique_ptr<int> i_zptr1;
		std::unique_ptr<const int> i_zptr2;
		std::unique_ptr<int> i_nullptr3;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_xptr1)(i_xptr2)(i_xptr3);
			iar(i_yptr1)(i_yptr2);
			iar(i_nullptr1)(i_nullptr2);

			iar(i_zptr1)(i_zptr2);
			iar(i_nullptr3);
		}

		CHECK_EQ(o_xptr1.get(), o_xptr2.get());
		CHECK_EQ(o_xptr1.get(), o_xptr3.get());
		CHECK_EQ(i_xptr1.get(), i_xptr2.get());
		CHECK_EQ(i_xptr1.get(), i_xptr3.get());
		CHECK_EQ(*i_xptr1, *i_xptr2);
		CHECK_EQ(*i_xptr1, *i_xptr3);

		CHECK_EQ(o_yptr1.get(), o_yptr2.get());
		CHECK_EQ(i_yptr1.get(), i_yptr2.get());
		CHECK_EQ(*i_yptr1, *i_yptr2);
		CHECK_UNARY_FALSE(i_nullptr1);
		CHECK_UNARY_FALSE(i_nullptr2);

		CHECK_EQ(*i_xptr1, *o_xptr1);
		CHECK_EQ(*i_xptr2, *o_xptr2);
		CHECK_EQ(*i_xptr3, *o_xptr3);

		CHECK_EQ(*i_zptr1, *o_zptr1);
		CHECK_EQ(*i_zptr2, *o_zptr2);
		CHECK_UNARY_FALSE(i_nullptr3);
	}
}

class TestClass {
public:
	TestClass(int v) : x(v) {
	}

	int x;

private:
	friend class vide::access;
	TestClass() = default;

	template <class Archive>
	void serialize(Archive& ar) { ar(x); }
};

template <class IArchive, class OArchive>
inline void test_default_construction() {
	auto o_ptr = std::make_shared<TestClass>(1);
	auto o_ptr2 = std::make_shared<const TestClass>(1);
	std::shared_ptr<TestClass> i_ptr;
	std::shared_ptr<const TestClass> i_ptr2;

	std::ostringstream os; {
		OArchive oar(os);
		oar(o_ptr);
		oar(o_ptr2);
	} {
		std::istringstream is(os.str());
		IArchive iar(is);
		iar(i_ptr);
		iar(i_ptr2);
	}
	CHECK_EQ(o_ptr->x, i_ptr->x);
	CHECK_EQ(o_ptr2->x, i_ptr2->x);
}

template <class IArchive, class OArchive>
inline void test_shared_ptr_type_mismatch() {
	auto o_ptr0 = std::make_shared<int>(42);
	auto o_ptr1 = o_ptr0;
	std::shared_ptr<int> i_ptr0;
	std::shared_ptr<double> i_ptr1; //< This will not be the same type as the data was saved with

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


TEST_SUITE_BEGIN("memory");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("memory", test_memory)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("default_construction", test_default_construction)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("shared_ptr_type_mismatch", test_shared_ptr_type_mismatch)

TEST_SUITE_END();
