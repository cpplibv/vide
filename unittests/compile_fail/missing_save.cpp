//
// Expected: could not find a serializer for \[NoSerializer\] with archive \[vide::BinaryOutputArchive\]

#include <vide/archives/binary.hpp>

#include <sstream>


struct NoSerializer {
	int x = 0;
};

void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(NoSerializer{});
}
