//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


#include <vide/types/std_variant.hpp>

template <class IArchive, class OArchive>
void test_std_variant() {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::variant<int, double, std::string> o_bv1 = random_value<int>(gen);
	std::variant<int, double, std::string> o_bv2 = random_value<double>(gen);
	std::variant<int, double, std::string> o_bv3 = random_basic_string<char>(gen);

	std::ostringstream os; {
		OArchive oar(os);

		oar(o_bv1);
		oar(o_bv2);
		oar(o_bv3);
	}

	decltype(o_bv1) i_bv1;
	decltype(o_bv2) i_bv2;
	decltype(o_bv3) i_bv3;

	std::istringstream is(os.str()); {
		IArchive iar(is);

		iar(i_bv1);
		iar(i_bv2);
		iar(i_bv3);
	}

	CHECK_EQ(std::get<int>(i_bv1), std::get<int>(o_bv1));
	CHECK_EQ(std::get<double>(i_bv2), doctest::Approx(std::get<double>(o_bv2)).epsilon(1e-5));
	CHECK_EQ(std::get<std::string>(i_bv3), std::get<std::string>(o_bv3));
}


TEST_SUITE_BEGIN("std_variant");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("std_variant", test_std_variant)

TEST_SUITE_END();
