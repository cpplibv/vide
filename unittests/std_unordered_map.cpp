//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_unordered_map() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		std::unordered_map<std::string, int> o_podunordered_map;
		for (int j = 0; j < 100; ++j)
			o_podunordered_map.insert({random_value<std::string>(gen), random_value<int>(gen)});

		std::unordered_map<uint16_t, StructInternalSerialize> o_iserunordered_map;
		for (int j = 0; j < 100; ++j)
			o_iserunordered_map.insert({random_value<uint16_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::unordered_map<uint16_t, StructInternalSplit> o_isplunordered_map;
		for (int j = 0; j < 100; ++j)
			o_isplunordered_map.insert({random_value<uint16_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::unordered_map<uint32_t, StructExternalSerialize> o_eserunordered_map;
		for (int j = 0; j < 100; ++j)
			o_eserunordered_map.insert({random_value<uint32_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::unordered_map<int8_t, StructExternalSplit> o_esplunordered_map;
		for (int j = 0; j < 100; ++j)
			o_esplunordered_map.insert({random_value<int8_t>(gen), {random_value<int>(gen), random_value<int>(gen)}});

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podunordered_map);
			oar(o_iserunordered_map);
			oar(o_isplunordered_map);
			oar(o_eserunordered_map);
			oar(o_esplunordered_map);
		}

		std::unordered_map<std::string, int> i_podunordered_map;
		std::unordered_map<uint16_t, StructInternalSerialize> i_iserunordered_map;
		std::unordered_map<uint16_t, StructInternalSplit> i_isplunordered_map;
		std::unordered_map<uint32_t, StructExternalSerialize> i_eserunordered_map;
		std::unordered_map<int8_t, StructExternalSplit> i_esplunordered_map;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podunordered_map);
			iar(i_iserunordered_map);
			iar(i_isplunordered_map);
			iar(i_eserunordered_map);
			iar(i_esplunordered_map);
		}

		for (auto const& p : i_podunordered_map) {
			auto v = o_podunordered_map.find(p.first);
			CHECK_NE(v, o_podunordered_map.end());
			CHECK_EQ(p.second, v->second);
		}

		for (auto const& p : i_iserunordered_map) {
			auto v = o_iserunordered_map.find(p.first);
			CHECK_NE(v, o_iserunordered_map.end());
			CHECK_EQ(p.second, v->second);
		}

		for (auto const& p : i_isplunordered_map) {
			auto v = o_isplunordered_map.find(p.first);
			CHECK_NE(v, o_isplunordered_map.end());
			CHECK_EQ(p.second, v->second);
		}

		for (auto const& p : i_eserunordered_map) {
			auto v = o_eserunordered_map.find(p.first);
			CHECK_NE(v, o_eserunordered_map.end());
			CHECK_EQ(p.second, v->second);
		}

		for (auto const& p : i_esplunordered_map) {
			auto v = o_esplunordered_map.find(p.first);
			CHECK_NE(v, o_esplunordered_map.end());
			CHECK_EQ(p.second, v->second);
		}
	}
}


TEST_SUITE_BEGIN("unordered_map");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("unordered_map", test_unordered_map)

TEST_SUITE_END();
