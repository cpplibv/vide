//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_multiset() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::multiset<int> o_podmultiset;
		for (int j = 0; j < 100; ++j) {
			int value = random_value<int>(gen);
			o_podmultiset.insert(value);
			o_podmultiset.insert(value);
		}

		std::multiset<StructInternalSerialize> o_isermultiset;
		for (int j = 0; j < 100; ++j) {
			StructInternalSerialize value = {random_value<int>(gen), random_value<int>(gen)};
			o_isermultiset.insert(value);
			o_isermultiset.insert(value);
		}

		std::multiset<StructInternalSplit> o_isplmultiset;
		for (int j = 0; j < 100; ++j) {
			StructInternalSplit value = {random_value<int>(gen), random_value<int>(gen)};
			o_isplmultiset.insert(value);
			o_isplmultiset.insert(value);
		}

		std::multiset<StructExternalSerialize> o_esermultiset;
		for (int j = 0; j < 100; ++j) {
			StructExternalSerialize value = {random_value<int>(gen), random_value<int>(gen)};
			o_esermultiset.insert(value);
			o_esermultiset.insert(value);
		}

		std::multiset<StructExternalSplit> o_esplmultiset;
		for (int j = 0; j < 100; ++j) {
			StructExternalSplit value = {random_value<int>(gen), random_value<int>(gen)};
			o_esplmultiset.insert(value);
			o_esplmultiset.insert(value);
		}

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podmultiset);
			oar(o_isermultiset);
			oar(o_isplmultiset);
			oar(o_esermultiset);
			oar(o_esplmultiset);
		}

		std::multiset<int> i_podmultiset;
		std::multiset<StructInternalSerialize> i_isermultiset;
		std::multiset<StructInternalSplit> i_isplmultiset;
		std::multiset<StructExternalSerialize> i_esermultiset;
		std::multiset<StructExternalSplit> i_esplmultiset;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podmultiset);
			iar(i_isermultiset);
			iar(i_isplmultiset);
			iar(i_esermultiset);
			iar(i_esplmultiset);
		}

		for (auto const& p : i_podmultiset) {
			CHECK_EQ(o_podmultiset.count(p), i_podmultiset.count(p));
		}

		for (auto const& p : i_isermultiset) {
			CHECK_EQ(o_isermultiset.count(p), i_isermultiset.count(p));
		}

		for (auto const& p : i_isplmultiset) {
			CHECK_EQ(o_isplmultiset.count(p), i_isplmultiset.count(p));
		}

		for (auto const& p : i_esermultiset) {
			CHECK_EQ(o_esermultiset.count(p), i_esermultiset.count(p));
		}

		for (auto const& p : i_esplmultiset) {
			CHECK_EQ(o_esplmultiset.count(p), i_esplmultiset.count(p));
		}
	}
}

// -------------------------------------------------------------------------------------------------

struct OrderChecker {
	int order;
	int value;

	template<class Archive>
	void serialize(Archive& ar) {
		ar.nvp("order", order);
		ar.nvp("value", value);
	}

	friend bool operator<(const OrderChecker& lhs, const OrderChecker& rhs) {
		return lhs.order < rhs.order;
	}

	friend bool operator==(const OrderChecker& lhs1, const OrderChecker& rhs1) = default;
};

template <class IArchive, class OArchive>
void test_multiset_preserve_order() {
	std::random_device rd;
	std::mt19937 gen(rd());

	SaveLoadTester<IArchive, OArchive> test{};

	std::multiset<OrderChecker> set;
	set.emplace(10, 1);
	set.emplace(10, 2);
	set.emplace(10, 3);
	set.emplace(10, 4);
	set.emplace(10, 5);
	set.emplace(10, 6);
	set.emplace(10, 7);
	set.emplace(10, 8);
	set.emplace(10, 9);
	CHECK(test(set));
}

TEST_SUITE_BEGIN("multiset");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("multiset", test_multiset)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("multiset_preserve_order", test_multiset_preserve_order)

TEST_SUITE_END();
