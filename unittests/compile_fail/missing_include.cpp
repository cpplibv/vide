//
// Expected: could not find a serializer for \[std::map<int, int>\] with archive \[vide::BinaryOutputArchive\]
// Expected: Most library types are covered by the vide/types/\* headers, is the matching one missing\?

#include <vide/archives/binary.hpp>

#include <map>
#include <sstream>


// vide/types/std_map.hpp is left out on purpose
void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(std::map<int, int>{});
}
