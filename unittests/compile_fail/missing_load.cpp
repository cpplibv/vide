//
// Expected: could not find a serializer for \[NoSerializer\] with archive \[vide::BinaryInputArchive\]

#include <vide/archives/binary.hpp>

#include <sstream>


struct NoSerializer {
	int x = 0;
};

void load_it() {
	std::istringstream is;
	vide::BinaryInputArchive ar(is);
	NoSerializer value;
	ar(value);
}
