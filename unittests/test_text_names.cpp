//

#include <vide/archives/json.hpp>
#include <vide/archives/xml.hpp>
#include <vide/base_class.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

#include "doctest.h"


TEST_SUITE_BEGIN("text_names");

// A name given to a value that writes nothing must not label the next unnamed value.
// Round trips cannot catch it: the input archive picks up the same stale name and still matches

namespace { // -------------------------------------------------------------------------------------

struct Base {
	int b = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("b", b);
	}
};

struct Left : virtual Base {
	int l = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(vide::make_nvp("base", vide::virtual_base_class<Base>(this)));
		ar.nvp("l", l);
	}
};

struct Right : virtual Base {
	int r = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		// Skipped: Left already wrote Base
		ar.nvp("base", vide::virtual_base_class<Base>(this));
		ar(r);
	}
};

struct Diamond : Left, Right {
	template <class Archive>
	void serialize(Archive& ar) {
		ar.template base_class<Left>(this);
		ar.template base_class<Right>(this);
	}
};

struct Deferring {
	int x = 0;
	int y = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(vide::make_nvp("x", vide::defer(x)));
		ar(y);
	}
};

struct Expected {
	const char* diamond;
	const char* deferring;
};

std::string without_whitespace(std::string text) {
	std::erase_if(text, [](unsigned char c) { return std::isspace(c); });
	return text;
}

template <class OArchive, class IArchive>
void test_skipped_names(const Expected& expected) {
	Diamond diamond;
	diamond.b = 1;
	diamond.l = 2;
	diamond.r = 3;
	Deferring deferring;
	deferring.x = 5;
	deferring.y = 6;

	std::stringstream ss;
	{
		OArchive ar(ss);
		ar.nvp("diamond", diamond);
		ar.nvp("deferring", deferring);
		ar.serializeDeferments();
	}

	const auto text = without_whitespace(ss.str());
	CHECK_MESSAGE(text.find(expected.diamond) != std::string::npos, text);
	CHECK_MESSAGE(text.find(expected.deferring) != std::string::npos, text);

	Diamond loadedDiamond;
	Deferring loadedDeferring;
	{
		IArchive ar(ss);
		ar.nvp("diamond", loadedDiamond);
		ar.nvp("deferring", loadedDeferring);
		ar.serializeDeferments();
	}

	CHECK_EQ(loadedDiamond.b, 1);
	CHECK_EQ(loadedDiamond.l, 2);
	CHECK_EQ(loadedDiamond.r, 3);
	CHECK_EQ(loadedDeferring.x, 5);
	CHECK_EQ(loadedDeferring.y, 6);
}

} // namespace -------------------------------------------------------------------------------------

TEST_CASE("json skipped names") {
	test_skipped_names<vide::JSONOutputArchive, vide::JSONInputArchive>({
			.diamond = R"("value1":{"value0":3})",
			.deferring = R"("deferring":{"value0":6})"});
}

TEST_CASE("xml skipped names") {
	test_skipped_names<vide::XMLOutputArchive, vide::XMLInputArchive>({
			.diamond = "<value1><value0>3</value0></value1>",
			.deferring = "<deferring><value0>6</value0></deferring>"});
}

TEST_SUITE_END();
