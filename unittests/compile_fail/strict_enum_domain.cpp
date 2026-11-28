//
// Expected: found no domain for the enum \[Color\], VIDE_STRICT_ENUM_DOMAIN requires one
// Expected: Specify exactly one domain

#define VIDE_STRICT_ENUM_DOMAIN 1

#include <vide/archives/binary.hpp>
#include <vide/types/enum.hpp>


enum class Color {
	red,
	green,
};

void save_it(vide::BinaryOutputArchive& ar, const Color& value) {
	ar(value);
}
