//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"
#include "common/my_intrusive_ptr_polymorphic.hpp"



struct TestRefCountedPolymorphicDerived : TestRefCountedPolymorphic {
	using TestRefCountedPolymorphic::TestRefCountedPolymorphic;

	template <typename Archive> void serialize(Archive& ar) {
		ar.template base_class<TestRefCountedPolymorphic>(this);
	}
};
VIDE_REGISTER_TYPE(TestRefCountedPolymorphicDerived)

template <class IArchive, class OArchive>
inline void test_polymorphic_custom_smart_ptr() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		my::intrusive_ptr<TestRefCountedPolymorphic> o_xptr1 = my::intrusive_ptr<TestRefCountedPolymorphic>(new TestRefCountedPolymorphic{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphic> o_xptr2 = o_xptr1;
		my::intrusive_ptr<const TestRefCountedPolymorphic> o_xptr3 = o_xptr1;
		my::intrusive_ptr<TestRefCountedPolymorphic> o_yptr1 = my::intrusive_ptr<TestRefCountedPolymorphic>(new TestRefCountedPolymorphic{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphic> o_yptr2 = o_yptr1;

		my::intrusive_ptr<TestRefCountedPolymorphic> outputA0 = my::intrusive_ptr<TestRefCountedPolymorphicDerived>(new TestRefCountedPolymorphicDerived{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphic> outputA1 = outputA0;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> outputB0 = my::intrusive_ptr<TestRefCountedPolymorphicDerived>(new TestRefCountedPolymorphicDerived{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> outputB1 = outputB0;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> outputC0 = my::intrusive_ptr<TestRefCountedPolymorphicDerived>(new TestRefCountedPolymorphicDerived{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphic> outputC1 = outputC0;
		my::intrusive_ptr<TestRefCountedPolymorphic> outputD0 = my::intrusive_ptr<TestRefCountedPolymorphicDerived>(new TestRefCountedPolymorphicDerived{random_value<int>(gen)});
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> outputD1 = my::intrusive_ptr<TestRefCountedPolymorphicDerived>(static_cast<TestRefCountedPolymorphicDerived*>(outputD0.get()));
		my::intrusive_ptr<TestRefCountedPolymorphic> o_nullptr1;
		my::intrusive_ptr<TestRefCountedPolymorphic> o_nullptr2;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_xptr1);
			oar(o_xptr2);
			oar(o_xptr3);
			oar(o_yptr1);
			oar(o_yptr2);
			oar(outputA0);
			oar(outputA1);
			oar(outputB0);
			oar(outputB1);
			oar(outputC0);
			oar(outputC1);
			oar(outputD0);
			oar(outputD1);
			oar(o_nullptr1);
			oar(o_nullptr2);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		my::intrusive_ptr<TestRefCountedPolymorphic> i_xptr1;
		my::intrusive_ptr<TestRefCountedPolymorphic> i_xptr2;
		my::intrusive_ptr<const TestRefCountedPolymorphic> i_xptr3;
		my::intrusive_ptr<TestRefCountedPolymorphic> i_yptr1;
		my::intrusive_ptr<TestRefCountedPolymorphic> i_yptr2;
		my::intrusive_ptr<TestRefCountedPolymorphic> inputA0;
		my::intrusive_ptr<TestRefCountedPolymorphic> inputA1;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> inputB0;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> inputB1;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> inputC0;
		my::intrusive_ptr<TestRefCountedPolymorphic> inputC1;
		my::intrusive_ptr<TestRefCountedPolymorphic> inputD0;
		my::intrusive_ptr<TestRefCountedPolymorphicDerived> inputD1;
		my::intrusive_ptr<TestRefCountedPolymorphic> i_nullptr1;
		my::intrusive_ptr<TestRefCountedPolymorphic> i_nullptr2;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_xptr1);
			iar(i_xptr2);
			iar(i_xptr3);
			iar(i_yptr1);
			iar(i_yptr2);
			iar(inputA0);
			iar(inputA1);
			iar(inputB0);
			iar(inputB1);
			iar(inputC0);
			iar(inputC1);
			iar(inputD0);
			iar(inputD1);
			iar(i_nullptr1);
			iar(i_nullptr2);
		}

		CHECK_EQ(o_xptr1.get(), o_xptr2.get());
		CHECK_EQ(o_xptr1.get(), o_xptr3.get());
		CHECK_EQ(o_xptr1.get(), o_xptr1.get());
		CHECK_EQ(outputA0.get(), outputA1.get());
		CHECK_EQ(outputB0.get(), outputB1.get());
		CHECK_EQ(outputC0.get(), outputC1.get());
		CHECK_EQ(outputD0.get(), outputD1.get());
		CHECK_EQ(o_yptr1.get(), o_yptr2.get());

		CHECK_EQ(i_xptr1.get(), i_xptr2.get());
		CHECK_EQ(i_xptr1.get(), i_xptr3.get());
		CHECK_EQ(i_yptr1.get(), i_yptr2.get());
		CHECK_EQ(inputA0.get(), inputA1.get());
		CHECK_EQ(inputB0.get(), inputB1.get());
		CHECK_EQ(inputC0.get(), inputC1.get());
		CHECK_EQ(inputD0.get(), inputD1.get());

		CHECK_EQ(*i_xptr1, *o_xptr1);
		CHECK_EQ(*i_xptr2, *o_xptr2);
		CHECK_EQ(*i_xptr3, *o_xptr3);
		CHECK_EQ(*i_yptr1, *o_yptr1);
		CHECK_EQ(*i_yptr2, *o_yptr2);
		CHECK_EQ(*inputA0, *outputA0);
		CHECK_EQ(*inputB0, *outputB0);
		CHECK_EQ(*inputC0, *outputC0);
		CHECK_EQ(*inputD0, *outputD0);

		CHECK_UNARY_FALSE(i_nullptr1);
		CHECK_UNARY_FALSE(i_nullptr2);
	}
}


TEST_SUITE_BEGIN("polymorphic_custom_smart_ptr");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_custom_smart_ptr", test_polymorphic_custom_smart_ptr)

TEST_SUITE_END();
