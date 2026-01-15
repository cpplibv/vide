//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


TEST_SUITE_BEGIN("unique_validation");


template<typename IArchive, typename OArchive>
struct Tester {
	template <typename T, typename K>
	auto operator()(const T& outputVar, K& inputVar) {
		std::ostringstream os; {
			OArchive oar(os);
			oar(outputVar);
		}

		std::istringstream is(os.str()); {
			IArchive iar(is);
			iar(inputVar);
		}

		return inputVar;
	}
};

template <typename IArchive, typename OArchive>
void test_unique_validation() {
	Tester<IArchive, OArchive> test{};

	// Using multi-containers to bypass unique constraints on saving
	std::multiset<int> outputValid0{0, 1, 2};
	std::multiset<int> outputInvalid0{0, 0, 1, 2};
	std::multimap<int, int> outputInvalid1{{0, 42}, {1, 42}, {1, 42}, {2, 42}};
	std::unordered_multiset<int> outputInvalid2{0, 0, 1, 2};
	std::unordered_multimap<int, int> outputInvalid3{{0, 42}, {1, 42}, {1, 42}, {2, 42}};

	std::set<int> intput0;
	std::map<int, int> intput1;
	std::unordered_set<int> intput2;
	std::unordered_map<int, int> intput3;

	CHECK_EQ(test(outputValid0, intput0), std::set<int>{0, 1, 2}); // This checks verifies that multi-containers bypass works
	CHECK_THROWS_AS(test(outputInvalid0, intput0), vide::Exception); // Real tests
	CHECK_THROWS_AS(test(outputInvalid1, intput1), vide::Exception);
	CHECK_THROWS_AS(test(outputInvalid2, intput2), vide::Exception);
	CHECK_THROWS_AS(test(outputInvalid3, intput3), vide::Exception);
}

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("unique_validation", test_unique_validation)

TEST_SUITE_END();
