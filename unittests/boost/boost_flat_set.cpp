//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../common.hpp"

#include <vide/types/boost_flat_set.hpp>


template <class IArchive, class OArchive>
void test_boost_flat_set() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		boost::container::flat_set<int> o_podset;
		for (int j = 0; j < 100; ++j)
			o_podset.insert(random_value<int>(gen));

		boost::container::flat_set<StructInternalSerialize> o_iserset;
		for (int j = 0; j < 100; ++j)
			o_iserset.insert({random_value<int>(gen), random_value<int>(gen)});

		boost::container::flat_set<StructInternalSplit> o_isplset;
		for (int j = 0; j < 100; ++j)
			o_isplset.insert({random_value<int>(gen), random_value<int>(gen)});

		boost::container::flat_set<StructExternalSerialize> o_eserset;
		for (int j = 0; j < 100; ++j)
			o_eserset.insert({random_value<int>(gen), random_value<int>(gen)});

		boost::container::flat_set<StructExternalSplit> o_esplset;
		for (int j = 0; j < 100; ++j)
			o_esplset.insert({random_value<int>(gen), random_value<int>(gen)});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podset);
			oar(o_iserset);
			oar(o_isplset);
			oar(o_eserset);
			oar(o_esplset);
		}

		boost::container::flat_set<int> i_podset;
		boost::container::flat_set<StructInternalSerialize> i_iserset;
		boost::container::flat_set<StructInternalSplit> i_isplset;
		boost::container::flat_set<StructExternalSerialize> i_eserset;
		boost::container::flat_set<StructExternalSplit> i_esplset;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podset);
			iar(i_iserset);
			iar(i_isplset);
			iar(i_eserset);
			iar(i_esplset);
		}

		check_collection(i_podset, o_podset);
		check_collection(i_iserset, o_iserset);
		check_collection(i_isplset, o_isplset);
		check_collection(i_eserset, o_eserset);
		check_collection(i_esplset, o_esplset);
	}
}


TEST_SUITE_BEGIN("boost_flat_set");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("boost_flat_set", test_boost_flat_set)

TEST_SUITE_END();
