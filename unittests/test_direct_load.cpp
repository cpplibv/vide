//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


TEST_SUITE_BEGIN("direct_load");

namespace { // -------------------------------------------------------------------------------------

struct Test {
	int v0;
	int v1;
	int loadNamed;
	int v2;
	int load;
	int v3;

	explicit Test(int value) {
		v0 = value;
		v1 = value + 1;
		loadNamed = value + 2;
		v2 = value + 3;
		load = value + 4;
		v3 = value + 5;
	}

	template <class Archive> void serialize(Archive& ar) {
		ar.nvp("v0", v0);
		ar.nvp("v1", v1);
		if constexpr (Archive::is_output)
			ar.nvp("loadNamed", loadNamed); // we only save it here, so we can direct load it during load
		else
			loadNamed = ar.template nvp_load<int>("loadNamed");
		ar.nvp("v2", v2);
		if constexpr (Archive::is_output)
			ar(load); // we only save it here, so we can direct load it during load
		else
			load = ar.template load<int>();
		ar.nvp("v3", v3);
	}

	[[nodiscard]] constexpr inline bool operator==(const Test&) const = default;
};

} // namespace -------------------------------------------------------------------------------------


template <typename IArchive, typename OArchive>
void test_direct_load() {
	Test output0{0};
	Test output1{1};
	Test output2{2};
	Test input0{0};
	Test input1{0};
	Test input2{0};

	std::ostringstream os; {
		OArchive oar(os);
		oar(output0);
		oar(output1);
		oar(output2);
	}

	std::istringstream is(os.str()); {
		IArchive iar(is);

		iar(input0);
		iar(input1);
		iar(input2);
	}

	CHECK_EQ(input0, output0);
	CHECK_EQ(input1, output1);
	CHECK_EQ(input2, output2);
}

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("enum", test_direct_load)

TEST_SUITE_END();
