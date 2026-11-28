//
// Expected: cannot serialize the raw pointer \[int\*\]
// Expected: use a smart pointer

#include <vide/archives/binary.hpp>


void save_it(vide::BinaryOutputArchive& ar, int* value) {
	ar(value);
}
