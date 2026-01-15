//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_queue() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::queue<int> o_podqueue;
		for (int j = 0; j < 100; ++j)
			o_podqueue.push(random_value<int>(gen));

		std::queue<StructInternalSerialize> o_iserqueue;
		for (int j = 0; j < 100; ++j)
			o_iserqueue.push({random_value<int>(gen), random_value<int>(gen)});

		std::queue<StructInternalSplit> o_isplqueue;
		for (int j = 0; j < 100; ++j)
			o_isplqueue.push({random_value<int>(gen), random_value<int>(gen)});

		std::queue<StructExternalSerialize> o_eserqueue;
		for (int j = 0; j < 100; ++j)
			o_eserqueue.push({random_value<int>(gen), random_value<int>(gen)});

		std::queue<StructExternalSplit> o_esplqueue;
		for (int j = 0; j < 100; ++j)
			o_esplqueue.push({random_value<int>(gen), random_value<int>(gen)});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podqueue);
			oar(o_iserqueue);
			oar(o_isplqueue);
			oar(o_eserqueue);
			oar(o_esplqueue);
		}

		std::queue<int> i_podqueue;
		std::queue<StructInternalSerialize> i_iserqueue;
		std::queue<StructInternalSplit> i_isplqueue;
		std::queue<StructExternalSerialize> i_eserqueue;
		std::queue<StructExternalSplit> i_esplqueue;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podqueue);
			iar(i_iserqueue);
			iar(i_isplqueue);
			iar(i_eserqueue);
			iar(i_esplqueue);
		}

		auto& i_podqueue_c = vide::queue_detail::container(i_podqueue);
		auto& i_iserqueue_c = vide::queue_detail::container(i_iserqueue);
		auto& i_isplqueue_c = vide::queue_detail::container(i_isplqueue);
		auto& i_eserqueue_c = vide::queue_detail::container(i_eserqueue);
		auto& i_esplqueue_c = vide::queue_detail::container(i_esplqueue);

		auto& o_podqueue_c = vide::queue_detail::container(o_podqueue);
		auto& o_iserqueue_c = vide::queue_detail::container(o_iserqueue);
		auto& o_isplqueue_c = vide::queue_detail::container(o_isplqueue);
		auto& o_eserqueue_c = vide::queue_detail::container(o_eserqueue);
		auto& o_esplqueue_c = vide::queue_detail::container(o_esplqueue);

		check_collection(i_podqueue_c, o_podqueue_c);
		check_collection(i_iserqueue_c, o_iserqueue_c);
		check_collection(i_isplqueue_c, o_isplqueue_c);
		check_collection(i_eserqueue_c, o_eserqueue_c);
		check_collection(i_esplqueue_c, o_esplqueue_c);
	}
}


TEST_SUITE_BEGIN("queue");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("queue", test_queue)

TEST_SUITE_END();
