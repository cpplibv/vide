//
// Expected: cannot serialize the incomplete type \[Incomplete\]

#include <vide/archives/binary.hpp>


struct Incomplete;

void save_it(vide::BinaryOutputArchive& ar, const Incomplete& value) {
	ar(value);
}
