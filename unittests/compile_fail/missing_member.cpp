//
// Expected: could not find a serializer for \[NoSerializer\] with archive \[vide::BinaryOutputArchive\]

#include <vide/archives/binary.hpp>

#include <sstream>


struct NoSerializer {
	int x = 0;
};

// The outer type is fine, the failure comes from one level down
struct Outer {
	NoSerializer inner;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("inner", inner);
	}
};

void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(Outer{});
}
