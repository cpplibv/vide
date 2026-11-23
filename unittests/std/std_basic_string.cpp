//

#include <vide/types/std_string.hpp>
#include <vide/types/std_tuple.hpp>

#include <cstdint>
#include <string>
#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"


TEST_CASE_TEMPLATE("std::basic_string", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("char") {
		check_roundtrip<Ar>(random_basic_string<char>(gen));
	}

	SUBCASE("empty") {
		check_roundtrip<Ar>(std::string{});
		check_roundtrip<Ar>(std::string(""));
	}
}

// Text archives store char strings only
TEST_CASE_TEMPLATE("std::basic_string of wide and unicode characters", Ar, VIDE_ALL_BINARY_ARCHIVES) {
	auto gen = test_rng();
	check_roundtrip<Ar>(random_basic_string<wchar_t>(gen));
	check_roundtrip<Ar>(random_basic_string<char16_t>(gen));
	check_roundtrip<Ar>(random_basic_string<char32_t>(gen));
}

// XML keeps leading and trailing whitespace and escapes markup characters (upstream issue 109)

TEST_CASE("xml string whitespace and markup") {
	for (const char* text : {"some text", "some text ", " some text", " some text ", "  ", "    text    ", " ]]> ",
			" &gt; > ]]> ", " < <]>] &lt; ", " &amp; &   "}) {
		CAPTURE(text);
		check_roundtrip<XMLArchives>(std::string(text));
	}
}

TEST_CASE("xml char whitespace and markup") {
	for (const char c : {' ', '\t', '\n', '\r', '&', '>', '<', '\'', '"', '!', '|'}) {
		CAPTURE(c);
		check_roundtrip<XMLArchives>(c);
		check_roundtrip<XMLArchives>(static_cast<std::uint8_t>(c));
		check_roundtrip<XMLArchives>(static_cast<std::int8_t>(c));
	}
}

TEST_CASE("xml strings in sequence") {
	// One archive, so each value meets the state the previous one left
	const std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string> strings{
			"some text", " some text", " some text ", "Long text without ws at the end", "some text ", " some text",
			" some text "};
	CHECK_EQ(roundtrip<XMLArchives>(strings), strings);
}
