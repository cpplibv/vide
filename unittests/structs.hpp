#pragma once

#include "common.hpp"


template <class IArchive, class OArchive> inline
void test_structs() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		StructInternalSerialize o_iser = {random_value<int>(gen), random_value<int>(gen)};
		StructInternalSplit o_ispl = {random_value<int>(gen), random_value<int>(gen)};
		StructExternalSerialize o_eser = {random_value<int>(gen), random_value<int>(gen)};
		StructExternalSplit o_espl = {random_value<int>(gen), random_value<int>(gen)};

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(o_iser);
			oar(o_ispl);
			oar(o_eser);
			oar(o_espl);
		}

		StructInternalSerialize i_iser;
		StructInternalSplit i_ispl;
		StructExternalSerialize i_eser;
		StructExternalSplit i_espl;

		std::istringstream is(os.str());
		{
			IArchive iar(is);
			iar(i_iser);
			iar(i_ispl);
			iar(i_eser);
			iar(i_espl);
		}

		CHECK_EQ(i_iser, o_iser);
		CHECK_EQ(i_ispl, o_ispl);
		CHECK_EQ(i_eser, o_eser);
		CHECK_EQ(i_espl, o_espl);
	}
}
