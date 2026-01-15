//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_stack() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::stack<int> o_podstack;
		for (int j = 0; j < 100; ++j)
			o_podstack.push(random_value<int>(gen));

		std::stack<StructInternalSerialize> o_iserstack;
		for (int j = 0; j < 100; ++j)
			o_iserstack.push({random_value<int>(gen), random_value<int>(gen)});

		std::stack<StructInternalSplit> o_isplstack;
		for (int j = 0; j < 100; ++j)
			o_isplstack.push({random_value<int>(gen), random_value<int>(gen)});

		std::stack<StructExternalSerialize> o_eserstack;
		for (int j = 0; j < 100; ++j)
			o_eserstack.push({random_value<int>(gen), random_value<int>(gen)});

		std::stack<StructExternalSplit> o_esplstack;
		for (int j = 0; j < 100; ++j)
			o_esplstack.push({random_value<int>(gen), random_value<int>(gen)});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podstack);
			oar(o_iserstack);
			oar(o_isplstack);
			oar(o_eserstack);
			oar(o_esplstack);
		}

		std::stack<int> i_podstack;
		std::stack<StructInternalSerialize> i_iserstack;
		std::stack<StructInternalSplit> i_isplstack;
		std::stack<StructExternalSerialize> i_eserstack;
		std::stack<StructExternalSplit> i_esplstack;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podstack);
			iar(i_iserstack);
			iar(i_isplstack);
			iar(i_eserstack);
			iar(i_esplstack);
		}

		auto& i_podstack_c = vide::stack_detail::container(i_podstack);
		auto& i_iserstack_c = vide::stack_detail::container(i_iserstack);
		auto& i_isplstack_c = vide::stack_detail::container(i_isplstack);
		auto& i_eserstack_c = vide::stack_detail::container(i_eserstack);
		auto& i_esplstack_c = vide::stack_detail::container(i_esplstack);

		auto& o_podstack_c = vide::stack_detail::container(o_podstack);
		auto& o_iserstack_c = vide::stack_detail::container(o_iserstack);
		auto& o_isplstack_c = vide::stack_detail::container(o_isplstack);
		auto& o_eserstack_c = vide::stack_detail::container(o_eserstack);
		auto& o_esplstack_c = vide::stack_detail::container(o_esplstack);

		check_collection(i_podstack_c, o_podstack_c);
		check_collection(i_iserstack_c, o_iserstack_c);
		check_collection(i_isplstack_c, o_isplstack_c);
		check_collection(i_eserstack_c, o_eserstack_c);
		check_collection(i_esplstack_c, o_esplstack_c);
	}
}


TEST_SUITE_BEGIN("stack");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("stack", test_stack)

TEST_SUITE_END();
