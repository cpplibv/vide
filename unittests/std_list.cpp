//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_list() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::list<int> o_podlist(100);
		for (auto& elem : o_podlist)
			elem = random_value<int>(gen);

		std::list<StructInternalSerialize> o_iserlist(100);
		for (auto& elem : o_iserlist)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::list<StructInternalSplit> o_ispllist(100);
		for (auto& elem : o_ispllist)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::list<StructExternalSerialize> o_eserlist(100);
		for (auto& elem : o_eserlist)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::list<StructExternalSplit> o_espllist(100);
		for (auto& elem : o_espllist)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podlist);
			oar(o_iserlist);
			oar(o_ispllist);
			oar(o_eserlist);
			oar(o_espllist);
		}

		std::list<int> i_podlist;
		std::list<StructInternalSerialize> i_iserlist;
		std::list<StructInternalSplit> i_ispllist;
		std::list<StructExternalSerialize> i_eserlist;
		std::list<StructExternalSplit> i_espllist;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podlist);
			iar(i_iserlist);
			iar(i_ispllist);
			iar(i_eserlist);
			iar(i_espllist);
		}

		check_collection(i_podlist, o_podlist);
		check_collection(i_iserlist, o_iserlist);
		check_collection(i_ispllist, o_ispllist);
		check_collection(i_eserlist, o_eserlist);
		check_collection(i_espllist, o_espllist);
	}
}


TEST_SUITE_BEGIN("list");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("list", test_list)

TEST_SUITE_END();
