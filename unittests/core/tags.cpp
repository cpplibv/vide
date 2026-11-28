//

#include <vide/details/vide_types.hpp>
#include <vide/exception.hpp>
#include <vide/types/std_string.hpp>

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/random.hpp"


// The archive member functions behind the tag types: binary_data(), size_tag() and defer()

TEST_CASE_TEMPLATE("binary_data", Ar, VIDE_ALL_BINARY_ARCHIVES) {
	auto gen = test_rng();
	const auto elements = random_container<std::vector<std::int16_t>>(gen, 100);

	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar.binary_data(elements.data(), elements.size());
	}
	// Portable binary leads with one byte of byte order
	CHECK_LE(os.str().size(), elements.size() * sizeof(std::int16_t) + 1); // size counts elements, not bytes

	std::vector<std::int16_t> loaded(elements.size());
	std::istringstream is(os.str());
	{
		typename Ar::input ar(is);
		ar.binary_data(loaded.data(), loaded.size());
	}
	CHECK_EQ(loaded, elements);
}

TEST_CASE_TEMPLATE("size_tag beyond size_type", Ar, VIDE_ALL_ARCHIVES) {
	constexpr std::uint64_t size_type_max = std::numeric_limits<vide::size_type>::max();

	std::ostringstream os;
	typename Ar::output ar(os);
	CHECK_THROWS_AS(ar.size_tag(size_type_max + 1), vide::Exception); // Instead of storing a truncated size
}

TEST_CASE_TEMPLATE("defer rvalue", Ar, VIDE_ALL_ARCHIVES) {
	const std::string text(64, 'x');

	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar.defer(std::string(text)); // The temporary is gone by serializeDeferments(): the tag holds a copy
		ar.serializeDeferments();
	}

	std::string loaded;
	std::istringstream is(os.str());
	{
		typename Ar::input ar(is);
		ar.defer(loaded);
		ar.serializeDeferments();
	}
	CHECK_EQ(loaded, text);
}
