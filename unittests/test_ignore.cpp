//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


TEST_SUITE_BEGIN("ignore");

namespace { // -------------------------------------------------------------------------------------

struct Test {
	int v0;
	int v1;
	int ignoreNamed;
	int v2;
	int ignore;
	int v3;

	explicit Test(int value) {
		v0 = value;
		v1 = value + 1;
		ignoreNamed = value + 2;
		v2 = value + 3;
		ignore = value + 4;
		v3 = value + 5;
	}

	template <class Archive> void serialize(Archive& ar) {
		ar.nvp("v0", v0);
		ar.nvp("v1", v1);
		if constexpr (Archive::is_output)
			ar.nvp("ignoreNamed", ignoreNamed); // we only save it here, so we can ignore it during load
		else {
			ar.template nvp_ignore<int>("ignoreNamed");
			ignoreNamed = v0 + 2;
		}
		ar.nvp("v2", v2);
		if constexpr (Archive::is_output)
			ar(ignore); // we only save it here, so we can ignore it during load
		else {
			ar.template ignore<int>();
			ignore = v0 + 4;
		}
		ar.nvp("v3", v3);
	}

	[[nodiscard]] constexpr inline bool operator==(const Test&) const = default;
};

} // namespace -------------------------------------------------------------------------------------


template <typename IArchive, typename OArchive>
void test_ignore() {
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

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("enum", test_ignore)

TEST_SUITE_END();
