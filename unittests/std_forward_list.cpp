//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_forward_list() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::forward_list<int> o_podforward_list(100);
		for (auto& elem : o_podforward_list)
			elem = random_value<int>(gen);

		std::forward_list<StructInternalSerialize> o_iserforward_list(100);
		for (auto& elem : o_iserforward_list)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::forward_list<StructInternalSplit> o_isplforward_list(100);
		for (auto& elem : o_isplforward_list)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::forward_list<StructExternalSerialize> o_eserforward_list(100);
		for (auto& elem : o_eserforward_list)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::forward_list<StructExternalSplit> o_esplforward_list(100);
		for (auto& elem : o_esplforward_list)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podforward_list);
			oar(o_iserforward_list);
			oar(o_isplforward_list);
			oar(o_eserforward_list);
			oar(o_esplforward_list);
		}

		std::forward_list<int> i_podforward_list;
		std::forward_list<StructInternalSerialize> i_iserforward_list;
		std::forward_list<StructInternalSplit> i_isplforward_list;
		std::forward_list<StructExternalSerialize> i_eserforward_list;
		std::forward_list<StructExternalSplit> i_esplforward_list;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podforward_list);
			iar(i_iserforward_list);
			iar(i_isplforward_list);
			iar(i_eserforward_list);
			iar(i_esplforward_list);
		}

		check_collection(i_podforward_list, o_podforward_list);
		check_collection(i_iserforward_list, o_iserforward_list);
		check_collection(i_isplforward_list, o_isplforward_list);
		check_collection(i_eserforward_list, o_eserforward_list);
		check_collection(i_esplforward_list, o_esplforward_list);
	}
}


TEST_SUITE_BEGIN("forward_list");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("forward_list", test_forward_list)

TEST_SUITE_END();
