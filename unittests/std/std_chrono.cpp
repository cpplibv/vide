//

#include <vide/types/std_chrono.hpp>

#include <chrono>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"


TEST_CASE_TEMPLATE("std::chrono", Ar, VIDE_ALL_ARCHIVES) {
	SUBCASE("time points") {
		check_roundtrip<Ar>(std::chrono::system_clock::now());
		check_roundtrip<Ar>(std::chrono::steady_clock::now());
		check_roundtrip<Ar>(std::chrono::high_resolution_clock::now());
	}

	SUBCASE("durations") {
		using namespace std::chrono_literals;
		check_roundtrip<Ar>(1234567ns);
		check_roundtrip<Ar>(-42ms);
		check_roundtrip<Ar>(std::chrono::duration<double>{1.5});
		check_roundtrip<Ar>(std::chrono::steady_clock::now().time_since_epoch());
	}
}
