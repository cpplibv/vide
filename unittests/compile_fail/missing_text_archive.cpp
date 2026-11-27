//
// Expected: could not find a serializer for \[NoSerializer\] with archive \[vide::JSONOutputArchive\]

#include <vide/archives/json.hpp>

#include <sstream>


struct NoSerializer {
	int x = 0;
};

void save_it() {
	std::ostringstream os;
	vide::JSONOutputArchive ar(os);
	ar.nvp("value", NoSerializer{});
}
