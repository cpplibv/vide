//

#include <vide/types/std_array.hpp>
#include <vide/types/std_deque.hpp>
#include <vide/types/std_vector.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <deque>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


struct BoolOwner {
	bool value = false;
	// 3 padding bytes
	std::array<bool, 4> array{false, false, false, false};
	std::vector<bool> vector{false, false, false, false};
	std::deque<bool> deque{false, false, false, false};

	BoolOwner() = default;

	/// Every bool holds raw as its byte; raw 2 is no valid bool (std::vector<bool> stores bits, it gets raw != 0)
	explicit BoolOwner(std::uint8_t raw) {
		std::memset(&value, raw, sizeof(value));
		for (auto& v : array)
			std::memset(&v, raw, sizeof(v));
		for (auto&& v : vector)
			v = raw != 0;
		for (auto& v : deque)
			std::memset(&v, raw, sizeof(v));
	}

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar.nvp("value", value);
		ar.nvp("array", array);
		ar.nvp("vector", vector);
		ar.nvp("deque", deque);
	}

	bool operator==(const BoolOwner&) const = default;
};

TEST_CASE_TEMPLATE("bool", Ar, VIDE_BINARY_ARCHIVES) {
	SUBCASE("valid values") {
		check_roundtrip<Ar, BoolOwner>(BoolOwner{0});
		check_roundtrip<Ar, BoolOwner>(BoolOwner{1});
	}

	SUBCASE("an invalid byte throws on load, naming its position") {
		// Offsets of the bool bytes in the data; the portable archive starts with one byte of endianness
		const int header = std::is_same_v<Ar, BinaryArchives> ? 0 : 1;
		for (const int bool_offset : {0, 1, 2, 3, 4, 9, 10, 11, 12, 17, 18, 19, 20}) {
			const int offset = bool_offset + header;
			CAPTURE(offset);
			std::string data = save<Ar>(BoolOwner{0});
			data[static_cast<std::size_t>(offset)] = 2;
			const std::string message = "Invalid bool value '2' at byte " + std::to_string(offset);
			CHECK_THROWS_WITH_AS((std::ignore = load<Ar, BoolOwner>(data)), message.c_str(), vide::Exception);
		}
	}
}
