//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_pair() {
	std::random_device rd;
	std::mt19937 gen(rd());

	auto rng = [&]() { return random_value<int>(gen); };

	for (int ii = 0; ii < 100; ++ii) {
		std::pair<int, int> o_podpair = {rng(), rng()};
		std::pair<StructInternalSerialize, StructInternalSerialize> o_iserpair = {{rng(), rng()}, {rng(), rng()}};
		std::pair<StructInternalSplit, StructInternalSplit> o_isplpair = {{rng(), rng()}, {rng(), rng()}};
		std::pair<StructExternalSerialize, StructExternalSerialize> o_eserpair = {{rng(), rng()}, {rng(), rng()}};
		std::pair<StructExternalSplit, StructExternalSplit> o_esplpair = {{rng(), rng()}, {rng(), rng()}};

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podpair);
			oar(o_iserpair);
			oar(o_isplpair);
			oar(o_eserpair);
			oar(o_esplpair);
		}

		std::pair<int, int> i_podpair;
		std::pair<StructInternalSerialize, StructInternalSerialize> i_iserpair;
		std::pair<StructInternalSplit, StructInternalSplit> i_isplpair;
		std::pair<StructExternalSerialize, StructExternalSerialize> i_eserpair;
		std::pair<StructExternalSplit, StructExternalSplit> i_esplpair;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podpair);
			iar(i_iserpair);
			iar(i_isplpair);
			iar(i_eserpair);
			iar(i_esplpair);
		}

		CHECK_EQ(i_podpair.first, o_podpair.first);
		CHECK_EQ(i_podpair.second, o_podpair.second);

		CHECK_EQ(i_iserpair.first, o_iserpair.first);
		CHECK_EQ(i_iserpair.second, o_iserpair.second);

		CHECK_EQ(i_isplpair.first, o_isplpair.first);
		CHECK_EQ(i_isplpair.second, o_isplpair.second);

		CHECK_EQ(i_eserpair.first, o_eserpair.first);
		CHECK_EQ(i_eserpair.second, o_eserpair.second);

		CHECK_EQ(i_esplpair.first, o_esplpair.first);
		CHECK_EQ(i_esplpair.second, o_esplpair.second);
	}
}


TEST_SUITE_BEGIN("pair");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("pair", test_pair)

TEST_SUITE_END();
