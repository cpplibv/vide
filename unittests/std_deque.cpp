//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_deque() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::deque<int> o_poddeque(100);
		for (auto& elem : o_poddeque)
			elem = random_value<int>(gen);

		std::deque<StructInternalSerialize> o_iserdeque(100);
		for (auto& elem : o_iserdeque)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::deque<StructInternalSplit> o_ispldeque(100);
		for (auto& elem : o_ispldeque)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::deque<StructExternalSerialize> o_eserdeque(100);
		for (auto& elem : o_eserdeque)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::deque<StructExternalSplit> o_espldeque(100);
		for (auto& elem : o_espldeque)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_poddeque);
			oar(o_iserdeque);
			oar(o_ispldeque);
			oar(o_eserdeque);
			oar(o_espldeque);
		}

		std::deque<int> i_poddeque;
		std::deque<StructInternalSerialize> i_iserdeque;
		std::deque<StructInternalSplit> i_ispldeque;
		std::deque<StructExternalSerialize> i_eserdeque;
		std::deque<StructExternalSplit> i_espldeque;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_poddeque);
			iar(i_iserdeque);
			iar(i_ispldeque);
			iar(i_eserdeque);
			iar(i_espldeque);
		}

		CHECK_EQ(i_poddeque.size(), o_poddeque.size());
		CHECK_EQ(i_iserdeque.size(), o_iserdeque.size());
		CHECK_EQ(i_ispldeque.size(), o_ispldeque.size());
		CHECK_EQ(i_eserdeque.size(), o_eserdeque.size());
		CHECK_EQ(i_espldeque.size(), o_espldeque.size());

		check_collection(i_poddeque, o_poddeque);
		check_collection(i_iserdeque, o_iserdeque);
		check_collection(i_ispldeque, o_ispldeque);
		check_collection(i_eserdeque, o_eserdeque);
		check_collection(i_espldeque, o_espldeque);
	}
}


TEST_SUITE_BEGIN("deque");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("deque", test_deque)

TEST_SUITE_END();
