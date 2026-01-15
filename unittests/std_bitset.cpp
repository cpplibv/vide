//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_bitset() {
	std::random_device rd;
	std::mt19937 gen(rd());

	auto rng32 = [&]() { return random_binary_string<32>(gen); };
	auto rng65 = [&]() { return random_binary_string<65>(gen); };
	auto rng256 = [&]() { return random_binary_string<256>(gen); };
	auto rng512 = [&]() { return random_binary_string<512>(gen); };

	for (int ii = 0; ii < 100; ++ii) {
		std::bitset<32> o_bit32(rng32());
		std::bitset<65> o_bit65(rng65());
		std::bitset<256> o_bit256(rng256());
		std::bitset<512> o_bit512(rng512());
		std::bitset<32> o_bit32_low(0);

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_bit32);
			oar(o_bit65);
			oar(o_bit256);
			oar(o_bit512);
			oar(o_bit32_low);
		}

		std::bitset<32> i_bit32;
		std::bitset<65> i_bit65;
		std::bitset<256> i_bit256;
		std::bitset<512> i_bit512;
		std::bitset<32> i_bit32_low(0xffffffff);

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_bit32);
			iar(i_bit65);
			iar(i_bit256);
			iar(i_bit512);
			iar(i_bit32_low);
		}

		CHECK_EQ(o_bit32, i_bit32);
		CHECK_EQ(o_bit65, i_bit65);
		CHECK_EQ(o_bit256, i_bit256);
		CHECK_EQ(o_bit512, i_bit512);

		CHECK_EQ(o_bit32_low, i_bit32_low);
	}
}


TEST_SUITE_BEGIN("bitset");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("bitset", test_bitset)

TEST_SUITE_END();
