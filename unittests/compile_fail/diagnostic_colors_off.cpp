//
// Expected raw: could not find a serializer for \[NoSerializer\] with archive \[vide::BinaryOutputArchive\]

// Top level builds enable the colors project wide
#undef VIDE_DIAGNOSTIC_COLORS
#define VIDE_DIAGNOSTIC_COLORS 0

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
