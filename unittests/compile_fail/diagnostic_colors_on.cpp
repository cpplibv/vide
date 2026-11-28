//
// Expected raw: could not find a serializer for \[.\[1;31mNoSerializer.\[0m\] with archive \[.\[1;31mvide::BinaryOutputArchive.\[0m\]

#define VIDE_DIAGNOSTIC_COLORS 1

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
