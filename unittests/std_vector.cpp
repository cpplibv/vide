//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_vector() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::vector<int> o_podvector(100);
		for (auto& elem : o_podvector)
			elem = random_value<int>(gen);

		std::vector<bool> o_boolvector;
		o_boolvector.resize(100);
		for (size_t i = 0; i < 100; ++i)
			o_boolvector[i] = (random_value<int>(gen) % 2) == 0;

		std::vector<StructInternalSerialize> o_iservector(100);
		for (auto& elem : o_iservector)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::vector<StructInternalSplit> o_isplvector(100);
		for (auto& elem : o_isplvector)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::vector<StructExternalSerialize> o_eservector(100);
		for (auto& elem : o_eservector)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::vector<StructExternalSplit> o_esplvector(100);
		for (auto& elem : o_esplvector)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podvector);
			oar(o_boolvector);
			oar(o_iservector);
			oar(o_isplvector);
			oar(o_eservector);
			oar(o_esplvector);
		}

		std::vector<int> i_podvector;
		std::vector<bool> i_boolvector;
		std::vector<StructInternalSerialize> i_iservector;
		std::vector<StructInternalSplit> i_isplvector;
		std::vector<StructExternalSerialize> i_eservector;
		std::vector<StructExternalSplit> i_esplvector;

		std::istringstream is(os.str(), std::ios::binary | std::ios::in); {
			IArchive iar(is);

			iar(i_podvector);
			iar(i_boolvector);
			iar(i_iservector);
			iar(i_isplvector);
			iar(i_eservector);
			iar(i_esplvector);
		}

		CHECK_EQ(i_podvector.size(), o_podvector.size());
		CHECK_EQ(i_boolvector.size(), o_boolvector.size());
		CHECK_EQ(i_iservector.size(), o_iservector.size());
		CHECK_EQ(i_isplvector.size(), o_isplvector.size());
		CHECK_EQ(i_eservector.size(), o_eservector.size());
		CHECK_EQ(i_esplvector.size(), o_esplvector.size());

		check_collection(i_podvector, o_podvector);
		check_collection(i_boolvector, o_boolvector);
		check_collection(i_iservector, o_iservector);
		check_collection(i_isplvector, o_isplvector);
		check_collection(i_eservector, o_eservector);
		check_collection(i_esplvector, o_esplvector);
	}
}


TEST_SUITE_BEGIN("vector");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("vector", test_vector)

TEST_SUITE_END();
