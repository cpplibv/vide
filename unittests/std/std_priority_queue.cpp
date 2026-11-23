//

#include <vide/types/std_queue.hpp>

#include <cstddef>
#include <queue>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

/// Elements in pop order; std::priority_queue has no operator==
template <typename T>
std::vector<T> drain(std::priority_queue<T> queue) {
	std::vector<T> elements;
	for (; !queue.empty(); queue.pop())
		elements.push_back(queue.top());
	return elements;
}

template <typename Ar, typename T>
void check_priority_queue_roundtrip(const std::priority_queue<T>& queue) {
	const CallLog calls;
	const auto loaded = roundtrip<Ar>(queue);
	CHECK_EQ(drain(loaded), drain(queue));
	check_once_each(calls, queue);
}

TEST_CASE_TEMPLATE("std::priority_queue", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_priority_queue_roundtrip<Ar>(random_container<std::priority_queue<int>>(gen, element_count));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_priority_queue_roundtrip<Ar>(random_container<std::priority_queue<S>>(gen, element_count));
		});
	}

	SUBCASE("empty") {
		check_priority_queue_roundtrip<Ar>(std::priority_queue<int>{});
	}
}
