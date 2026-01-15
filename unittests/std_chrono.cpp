//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_chrono() {
	for (int ii = 0; ii < 100; ++ii) {
		auto o_timePoint1 = std::chrono::system_clock::now();
		auto o_timePoint2 = std::chrono::steady_clock::now();
		auto o_timePoint3 = std::chrono::high_resolution_clock::now();

		auto o_duration1 = std::chrono::system_clock::now() - o_timePoint1;
		auto o_duration2 = std::chrono::steady_clock::now() - o_timePoint2;
		auto o_duration3 = std::chrono::high_resolution_clock::now() - o_timePoint3;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_timePoint1);
			oar(o_timePoint2);
			oar(o_timePoint3);
			oar(o_duration1);
			oar(o_duration2);
			oar(o_duration3);
		}

		decltype(o_timePoint1) i_timePoint1;
		decltype(o_timePoint2) i_timePoint2;
		decltype(o_timePoint3) i_timePoint3;
		decltype(o_duration1) i_duration1;
		decltype(o_duration2) i_duration2;
		decltype(o_duration3) i_duration3;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_timePoint1);
			iar(i_timePoint2);
			iar(i_timePoint3);
			iar(i_duration1);
			iar(i_duration2);
			iar(i_duration3);
		}

		CHECK_EQ(o_timePoint1, i_timePoint1);
		CHECK_EQ(o_timePoint2, i_timePoint2);
		CHECK_EQ(o_timePoint3, i_timePoint3);
		CHECK_EQ(o_duration1, i_duration1);
		CHECK_EQ(o_duration2, i_duration2);
		CHECK_EQ(o_duration3, i_duration3);
	}
}


TEST_SUITE_BEGIN("chrono");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("chrono", test_chrono)

TEST_SUITE_END();
