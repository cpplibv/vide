//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_complex() {
	std::random_device rd;
	std::mt19937 gen(rd());

	auto rngF = [&]() { return random_value<float>(gen); };
	auto rngD = [&]() { return random_value<double>(gen); };
	auto rngLD = [&]() { return random_value<long double>(gen); };

	for (int ii = 0; ii < 100; ++ii) {
		std::complex<float> o_float(rngF(), rngF());
		std::complex<double> o_double(rngD(), rngD());
		std::complex<long double> o_ldouble(rngLD(), rngLD());

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_float);
			oar(o_double);
			oar(o_ldouble);
		}

		std::complex<float> i_float;
		std::complex<double> i_double;
		std::complex<long double> i_ldouble;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_float);
			iar(i_double);
			iar(i_ldouble);
		}

		CHECK_EQ(o_float, i_float);
		CHECK_EQ(o_double.real(), doctest::Approx(i_double.real()).epsilon(1e-5));
		CHECK_EQ(o_double.imag(), doctest::Approx(i_double.imag()).epsilon(1e-5));
		CHECK_EQ(o_ldouble.real(), doctest::Approx(i_ldouble.real()).epsilon(1e-5L));
		CHECK_EQ(o_ldouble.imag(), doctest::Approx(i_ldouble.imag()).epsilon(1e-5L));
	}
}


TEST_SUITE_BEGIN("complex");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("complex", test_complex)

TEST_SUITE_END();
