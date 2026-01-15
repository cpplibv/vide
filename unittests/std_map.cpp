//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_map() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::map<size_t, std::vector<StructInternalSerialize>> o_vectormap;
		for (int j = 0; j < 10; ++j) {
			size_t id = random_value<size_t>(gen);
			for (int k = 0; k < 100; ++k)
				o_vectormap[id].emplace_back(random_value<int>(gen), random_value<int>(gen));
		}

		std::map<std::string, int> o_podmap;
		for (int j = 0; j < 100; ++j)
			o_podmap.insert({random_value<std::string>(gen), random_value<int>(gen)});

		std::map<int, StructInternalSerialize> o_isermap;
		for (int j = 0; j < 100; ++j)
			o_isermap.insert({random_value<int>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::map<int, StructInternalSplit> o_isplmap;
		for (int j = 0; j < 100; ++j)
			o_isplmap.insert({random_value<int>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::map<uint32_t, StructExternalSerialize> o_esermap;
		for (int j = 0; j < 100; ++j)
			o_esermap.insert({random_value<uint32_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::map<int8_t, StructExternalSplit> o_esplmap;
		for (int j = 0; j < 100; ++j)
			o_esplmap.insert({random_value<int8_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_vectormap);
			oar(o_podmap);
			oar(o_isermap);
			oar(o_isplmap);
			oar(o_esermap);
			oar(o_esplmap);
		}

		std::map<size_t, std::vector<StructInternalSerialize>> i_vectormap;
		std::map<std::string, int> i_podmap;
		std::map<int, StructInternalSerialize> i_isermap;
		std::map<int, StructInternalSplit> i_isplmap;
		std::map<uint32_t, StructExternalSerialize> i_esermap;
		std::map<int8_t, StructExternalSplit> i_esplmap;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_vectormap);
			iar(i_podmap);
			iar(i_isermap);
			iar(i_isplmap);
			iar(i_esermap);
			iar(i_esplmap);
		}

		CHECK_EQ(i_vectormap.size(), o_vectormap.size());
		auto o_v_it = o_vectormap.begin();
		auto i_v_it = i_vectormap.begin();
		for (; o_v_it != o_vectormap.end(); ++o_v_it, ++i_v_it) {
			CHECK_EQ(i_v_it->second.size(), o_v_it->second.size());
			check_collection(i_v_it->second, o_v_it->second);
		}

		check_collection(i_podmap, o_podmap);
		check_collection(i_isermap, o_isermap);
		check_collection(i_isplmap, o_isplmap);
		check_collection(i_esermap, o_esermap);
		check_collection(i_esplmap, o_esplmap);
	}
}

template <class IArchive, class OArchive>
void test_map_memory() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::map<int, std::unique_ptr<int>> o_uniqueptrMap;
		std::map<int, std::shared_ptr<int>> o_sharedptrMap;

		for (int j = 0; j < 100; ++j) {
			o_uniqueptrMap.emplace(random_value<int>(gen), std::unique_ptr<int>(new int(random_value<int>(gen))));
			o_sharedptrMap.emplace(random_value<int>(gen), std::make_shared<int>(random_value<int>(gen)));
		}

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_uniqueptrMap);
			oar(o_sharedptrMap);
		}

		decltype( o_uniqueptrMap ) i_uniqueptrMap;
		decltype( o_sharedptrMap ) i_sharedptrMap;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_uniqueptrMap);
			iar(i_sharedptrMap);
		}

		CHECK_EQ(o_sharedptrMap.size(), i_sharedptrMap.size());
		CHECK_EQ(o_uniqueptrMap.size(), i_uniqueptrMap.size());

		auto o_v_it = o_uniqueptrMap.begin();
		auto i_v_it = i_uniqueptrMap.begin();
		for (; o_v_it != o_uniqueptrMap.end(); ++o_v_it, ++i_v_it) {
			CHECK_EQ(i_v_it->first, o_v_it->first);
			CHECK_EQ(*i_v_it->second, *o_v_it->second);
		}
	}
}


TEST_SUITE_BEGIN("map");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("map", test_map)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("map_memory", test_map_memory)

TEST_SUITE_END();
