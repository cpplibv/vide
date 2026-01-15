//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_tuple() {
	std::random_device rd;
	std::mt19937 gen(rd());

	auto rng = [&]() { return random_value<int>(gen); };

	for (int ii = 0; ii < 100; ++ii) {
		auto o_podtuple = std::make_tuple(rng(), rng(), rng(), rng());
		auto o_podtuple11 = std::make_tuple(rng(), rng(), rng(), rng(), rng(), rng(),
				rng(), rng(), rng(), rng(), rng());
		auto o_isertuple = std::make_tuple(StructInternalSerialize(rng(), rng()),
				StructInternalSerialize(rng(), rng()),
				StructInternalSerialize(rng(), rng()),
				StructInternalSerialize(rng(), rng()));
		auto o_ispltuple = std::make_tuple(StructInternalSplit(rng(), rng()),
				StructInternalSplit(rng(), rng()),
				StructInternalSplit(rng(), rng()),
				StructInternalSplit(rng(), rng()));
		auto o_esertuple = std::make_tuple(StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()));
		auto o_espltuple = std::make_tuple(StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()),
				StructExternalSerialize(rng(), rng()));

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_podtuple);
			oar(o_podtuple11);
			oar(o_isertuple);
			oar(o_ispltuple);
			oar(o_esertuple);
			oar(o_espltuple);
		}

		decltype( o_podtuple ) i_podtuple;
		decltype( o_podtuple11 ) i_podtuple11;
		decltype( o_isertuple ) i_isertuple;
		decltype( o_ispltuple ) i_ispltuple;
		decltype( o_esertuple ) i_esertuple;
		decltype( o_espltuple ) i_espltuple;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_podtuple);
			iar(i_podtuple11);
			iar(i_isertuple);
			iar(i_ispltuple);
			iar(i_esertuple);
			iar(i_espltuple);
		}

		CHECK_EQ(i_podtuple, o_podtuple);
		CHECK_EQ(i_podtuple11, o_podtuple11);
		CHECK_EQ(i_isertuple, o_isertuple);
		CHECK_EQ(i_ispltuple, o_ispltuple);
		CHECK_EQ(i_esertuple, o_esertuple);
		CHECK_EQ(i_espltuple, o_espltuple);
	}
}


TEST_SUITE_BEGIN("tuple");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("tuple", test_tuple)

TEST_SUITE_END();
