//
// Expected: could not find a serializer for \[NoSerializer\] with archive \[vide::BinaryOutputArchive\]

#include <vide/archives/binary.hpp>
#include <vide/types/std_vector.hpp>

#include <sstream>
#include <vector>


struct NoSerializer {
	int x = 0;
};

void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(std::vector<NoSerializer>(1));
}
