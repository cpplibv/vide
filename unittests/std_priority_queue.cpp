//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_priority_queue() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::priority_queue<int> o_podpriority_queue;
		for (int j = 0; j < 100; ++j)
			o_podpriority_queue.push(random_value<int>(gen));

		std::priority_queue<StructInternalSerialize> o_iserpriority_queue;
		for (int j = 0; j < 100; ++j)
			o_iserpriority_queue.push({random_value<int>(gen), random_value<int>(gen)});

		std::priority_queue<StructInternalSplit> o_isplpriority_queue;
		for (int j = 0; j < 100; ++j)
			o_isplpriority_queue.push({random_value<int>(gen), random_value<int>(gen)});

		std::priority_queue<StructExternalSerialize> o_eserpriority_queue;
		for (int j = 0; j < 100; ++j)
			o_eserpriority_queue.push({random_value<int>(gen), random_value<int>(gen)});

		std::priority_queue<StructExternalSplit> o_esplpriority_queue;
		for (int j = 0; j < 100; ++j)
			o_esplpriority_queue.push({random_value<int>(gen), random_value<int>(gen)});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podpriority_queue);
			oar(o_iserpriority_queue);
			oar(o_isplpriority_queue);
			oar(o_eserpriority_queue);
			oar(o_esplpriority_queue);
		}

		std::priority_queue<int> i_podpriority_queue;
		std::priority_queue<StructInternalSerialize> i_iserpriority_queue;
		std::priority_queue<StructInternalSplit> i_isplpriority_queue;
		std::priority_queue<StructExternalSerialize> i_eserpriority_queue;
		std::priority_queue<StructExternalSplit> i_esplpriority_queue;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podpriority_queue);
			iar(i_iserpriority_queue);
			iar(i_isplpriority_queue);
			iar(i_eserpriority_queue);
			iar(i_esplpriority_queue);
		}

		auto& i_podpriority_queue_c = vide::queue_detail::container(i_podpriority_queue);
		auto& i_iserpriority_queue_c = vide::queue_detail::container(i_iserpriority_queue);
		auto& i_isplpriority_queue_c = vide::queue_detail::container(i_isplpriority_queue);
		auto& i_eserpriority_queue_c = vide::queue_detail::container(i_eserpriority_queue);
		auto& i_esplpriority_queue_c = vide::queue_detail::container(i_esplpriority_queue);

		auto& o_podpriority_queue_c = vide::queue_detail::container(o_podpriority_queue);
		auto& o_iserpriority_queue_c = vide::queue_detail::container(o_iserpriority_queue);
		auto& o_isplpriority_queue_c = vide::queue_detail::container(o_isplpriority_queue);
		auto& o_eserpriority_queue_c = vide::queue_detail::container(o_eserpriority_queue);
		auto& o_esplpriority_queue_c = vide::queue_detail::container(o_esplpriority_queue);

		check_collection(i_podpriority_queue_c, o_podpriority_queue_c);
		check_collection(i_iserpriority_queue_c, o_iserpriority_queue_c);
		check_collection(i_isplpriority_queue_c, o_isplpriority_queue_c);
		check_collection(i_eserpriority_queue_c, o_eserpriority_queue_c);
		check_collection(i_esplpriority_queue_c, o_esplpriority_queue_c);
	}
}


TEST_SUITE_BEGIN("priority_queue");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("priority_queue", test_priority_queue)

TEST_SUITE_END();
