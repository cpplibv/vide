//
// Expected: cannot serialize the enum \[Opaque\] from an opaque declaration

#include <vide/archives/binary.hpp>
#include <vide/types/enum.hpp>


enum class Opaque : int;

void save_it(vide::BinaryOutputArchive& ar, const Opaque& value) {
	ar(value);
}
