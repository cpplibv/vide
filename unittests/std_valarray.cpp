//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
inline
void test_valarray() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::valarray<int> o_podvalarray(100);
		for (auto& elem : o_podvalarray)
			elem = random_value<int>(gen);

		std::valarray<StructInternalSerialize> o_iservalarray(100);
		for (auto& elem : o_iservalarray)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::valarray<StructInternalSplit> o_isplvalarray(100);
		for (auto& elem : o_isplvalarray)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::valarray<StructExternalSerialize> o_eservalarray(100);
		for (auto& elem : o_eservalarray)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::valarray<StructExternalSplit> o_esplvalarray(100);
		for (auto& elem : o_esplvalarray)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podvalarray);
			oar(o_iservalarray);
			oar(o_isplvalarray);
			oar(o_eservalarray);
			oar(o_esplvalarray);
		}

		std::valarray<int> i_podvalarray;
		std::valarray<StructInternalSerialize> i_iservalarray;
		std::valarray<StructInternalSplit> i_isplvalarray;
		std::valarray<StructExternalSerialize> i_eservalarray;
		std::valarray<StructExternalSplit> i_esplvalarray;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podvalarray);
			iar(i_iservalarray);
			iar(i_isplvalarray);
			iar(i_eservalarray);
			iar(i_esplvalarray);
		}

		CHECK_EQ(i_podvalarray.size(), o_podvalarray.size());
		CHECK_EQ(i_iservalarray.size(), o_iservalarray.size());
		CHECK_EQ(i_isplvalarray.size(), o_isplvalarray.size());
		CHECK_EQ(i_eservalarray.size(), o_eservalarray.size());
		CHECK_EQ(i_esplvalarray.size(), o_esplvalarray.size());

		check_collection(i_podvalarray, o_podvalarray);
		check_collection(i_iservalarray, o_iservalarray);
		check_collection(i_isplvalarray, o_isplvalarray);
		check_collection(i_eservalarray, o_eservalarray);
		check_collection(i_esplvalarray, o_esplvalarray);
	}
}


TEST_SUITE_BEGIN("valarray");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("valarray", test_valarray)

TEST_SUITE_END();
