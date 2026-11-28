//
// Expected: more than one domain for the enum \[Color\]: serialize_end_value, serialize_max_value\.
// Expected: Specify exactly one domain

#include <vide/archives/binary.hpp>
#include <vide/types/enum.hpp>


enum class Color {
	red,
	green,
	serialize_end_value,
	serialize_max_value = green,
};

void save_it(vide::BinaryOutputArchive& ar, const Color& value) {
	ar(value);
}
