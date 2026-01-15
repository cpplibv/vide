//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


template <class IArchive, class OArchive>
void test_string_basic() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (size_t i = 0; i < 100; ++i) {
		std::basic_string<char> o_string = random_basic_string<char>(gen);
		std::basic_string<char> o_string2 = "";
		std::basic_string<char> o_string3;

		std::ostringstream os; {
			OArchive oar(os);
			oar(o_string);
			oar(o_string2);
			oar(o_string3);
		}

		std::basic_string<char> i_string;
		std::basic_string<char> i_string2;
		std::basic_string<char> i_string3;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_string);
			iar(i_string2);
			iar(i_string3);
		}

		CHECK_EQ(i_string, o_string);
		CHECK_EQ(i_string2, o_string2);
		CHECK_EQ(i_string3, o_string3);
	}
}

template <class IArchive, class OArchive>
void test_string_all() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (size_t i = 0; i < 100; ++i) {
		std::basic_string<char> o_string = random_basic_string<char>(gen);
		std::basic_string<wchar_t> o_wstring = random_basic_string<wchar_t>(gen);
		std::basic_string<char16_t> o_u16string = random_basic_string<char16_t>(gen);
		std::basic_string<char32_t> o_u32string = random_basic_string<char32_t>(gen);

		std::ostringstream os; {
			OArchive oar(os);
			oar(o_string);
			oar(o_wstring);
			oar(o_u16string);
			oar(o_u32string);
		}

		std::basic_string<char> i_string;
		std::basic_string<wchar_t> i_wstring;
		std::basic_string<char16_t> i_u16string;
		std::basic_string<char32_t> i_u32string;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_string);
			iar(i_wstring);
			iar(i_u16string);
			iar(i_u32string);
		}

		CHECK_EQ(i_string, o_string);
		check_collection(i_wstring, o_wstring);
		check_collection(i_u16string, o_u16string);
		check_collection(i_u32string, o_u32string);
	}
}


TEST_SUITE_BEGIN("basic_string");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("string_basic", test_string_basic)

template <class IArchive, class OArchive, class Out, class In = Out>
void test_ws_in_out(Out const& o_value_with_ws) {
	std::ostringstream os; {
		OArchive oar(os);
		oar(o_value_with_ws);
	}

	In i_value_with_ws;

	std::istringstream is(os.str()); {
		IArchive iar(is);
		iar(i_value_with_ws);
	}

	CHECK(i_value_with_ws == o_value_with_ws);
}

TEST_CASE("xml_string_issue109") {
	char strings[][20] = {
		"some text",
		"some text ",
		" some text",
		" some text ",
		"  ",
		"    text    ",
		" ]]> ",
		" &gt; > ]]> ",
		" < <]>] &lt; ",
		" &amp; &   "
	};

	for (size_t i = 0; i < (sizeof(strings) / sizeof(strings[0])); ++i) {
		std::basic_string<char> o_string = strings[i];

		test_ws_in_out<vide::XMLInputArchive, vide::XMLOutputArchive>(o_string);
	}
}

TEST_CASE("xml_char_issue109") {
	uint8_t chars[] = {
		' ',
		'\t',
		'\n',
		'\r',
		'&',
		'>',
		'<',
		'\'',
		'"',
		'!',
		'|'
	};

	for (size_t i = 0; i < (sizeof(chars) / sizeof(chars[0])); ++i) {
		test_ws_in_out<vide::XMLInputArchive, vide::XMLOutputArchive>(chars[i]);
	}

	for (size_t i = 0; i < (sizeof(chars) / sizeof(chars[0])); ++i) {
		test_ws_in_out<vide::XMLInputArchive, vide::XMLOutputArchive>(int8_t(chars[i]));
	}

	for (size_t i = 0; i < (sizeof(chars) / sizeof(chars[0])); ++i) {
		test_ws_in_out<vide::XMLInputArchive, vide::XMLOutputArchive>(char(chars[i]));
	}
}

template <class IArchive, class OArchive, class Out, size_t Nb, class In = Out>
void test_ws_in_out_array(Out const (&o_a_value_with_ws)[Nb]) {
	std::ostringstream os; {
		OArchive oar(os);
		for (const auto& o_value_with_ws : o_a_value_with_ws) {
			oar(o_value_with_ws);
		}
	}

	In i_a_value_with_ws[Nb];

	std::istringstream is(os.str()); {
		IArchive iar(is);
		for (In& i_value_with_ws : i_a_value_with_ws) {
			iar(i_value_with_ws);
		}
	}

	for (size_t uiIndex = 0; uiIndex < Nb; ++uiIndex) {
		CHECK(i_a_value_with_ws[uiIndex] == o_a_value_with_ws[uiIndex]);
	}
}

TEST_CASE("xml_string_issue_consecutive_calls") {
	std::string strings[] = {
		"some text",
		" some text",
		" some text ",
		"Long text without ws at the end",
		"some text ",
		" some text",
		" some text ",
	};

	test_ws_in_out_array<vide::XMLInputArchive, vide::XMLOutputArchive>(strings);
}

TEST_SUITE_END();
