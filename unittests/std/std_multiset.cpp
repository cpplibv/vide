//

#include <vide/types/std_set.hpp>

#include <cstddef>
#include <set>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

/// Every random element twice, so duplicates are present
template <typename T>
std::multiset<T> random_multiset_with_duplicates(std::mt19937& gen) {
	std::multiset<T> values;
	for (std::size_t i = 0; i < element_count; ++i) {
		const T value = random_value<T>(gen);
		values.insert(value);
		values.insert(value);
	}
	return values;
}

/// Equal by order but told apart by value, so a reordering of equal elements shows
struct OrderChecker {
	int order = 0;
	int value = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("order", order);
		ar.nvp("value", value);
	}

	friend bool operator<(const OrderChecker& lhs, const OrderChecker& rhs) {
		return lhs.order < rhs.order;
	}

	friend bool operator==(const OrderChecker&, const OrderChecker&) = default;
};

TEST_CASE_TEMPLATE("std::multiset", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("duplicates") {
		const auto ints = random_multiset_with_duplicates<int>(gen);
		check_roundtrip<Ar>(ints);

		for_each_sample_struct([&]<typename S> {
			const auto structs = random_multiset_with_duplicates<S>(gen);
			check_roundtrip<Ar>(structs);
		});
	}

	SUBCASE("equal elements keep their order") {
		std::multiset<OrderChecker> ordered;
		for (int value = 1; value <= 9; ++value)
			ordered.emplace(10, value);
		check_roundtrip<Ar>(ordered);
	}
}
