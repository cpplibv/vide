//

#pragma once

#include "common.hpp"

template <typename IArchive, typename OArchive, size_t N>
inline void aux_test_array() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (size_t ii = 0; ii < N; ++ii) {
		std::array<int, N> o_podarray;
		for (auto& elem : o_podarray)
			elem = random_value<int>(gen);

		std::array<StructInternalSerialize, N> o_iserarray;
		for (auto& elem : o_iserarray)
			elem = StructInternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::array<StructInternalSplit, N> o_isplarray;
		for (auto& elem : o_isplarray)
			elem = StructInternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::array<StructExternalSerialize, N> o_eserarray;
		for (auto& elem : o_eserarray)
			elem = StructExternalSerialize(random_value<int>(gen), random_value<int>(gen));

		std::array<StructExternalSplit, N> o_esplarray;
		for (auto& elem : o_esplarray)
			elem = StructExternalSplit(random_value<int>(gen), random_value<int>(gen));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podarray);
			oar(o_iserarray);
			oar(o_isplarray);
			oar(o_eserarray);
			oar(o_esplarray);
		}

		std::array<int, N> i_podarray;
		std::array<StructInternalSerialize, N> i_iserarray;
		std::array<StructInternalSplit, N> i_isplarray;
		std::array<StructExternalSerialize, N> i_eserarray;
		std::array<StructExternalSplit, N> i_esplarray;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podarray);
			iar(i_iserarray);
			iar(i_isplarray);
			iar(i_eserarray);
			iar(i_esplarray);
		}

		check_collection(i_podarray, o_podarray);
		check_collection(i_iserarray, o_iserarray);
		check_collection(i_isplarray, o_isplarray);
		check_collection(i_eserarray, o_eserarray);
		check_collection(i_esplarray, o_esplarray);
	}
}

template <typename IArchive, typename OArchive>
inline void test_array() {
	aux_test_array<IArchive, OArchive, 0>();
	aux_test_array<IArchive, OArchive, 100>();
}
