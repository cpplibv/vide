"""Generates a serializer dispatch benchmark TU: N distinct user structs, every one round-tripped
through all four archives. Not part of the CMake build on purpose.

    python3 gen_dispatch_bench.py <mode> <count> > bench.cpp
    mode: member - member serialize(Archive&), builds against any vide version
          spec   - vide::serializer<T> specialization (vide 3.1+ serializer entry point)
          free   - free serialize(Archive&, T&) found by ADL (vide before the serializer entry point)
"""
import sys

# Nesting depth per chain; longer chains hit GCC's template depth limit
chain_length = 20

mode, count = sys.argv[1], int(sys.argv[2])
assert mode in ("free", "member", "spec")

out = ["""#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>
#include <vide/types/enum.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <cstdint>
#include <sstream>
#include <vector>

namespace user {

enum class Color : std::uint8_t { red, green, blue };
"""]

body = lambda ar, v: f'{ar}.nvp("a", {v}a).nvp("b", {v}b).nvp("c", {v}c).nvp("d", {v}d)' + (f'.nvp("prev", {v}prev)' if i % chain_length else '')

for i in range(count):
    prev = f"\tS{i - 1} prev;\n" if i % chain_length else ""
    out.append(f"struct S{i} {{\n\tint a = {i};\n\tdouble b = 0.5;\n\tstd::vector<int> c{{1, 2, 3}};\n\tColor d = Color::green;\n{prev}")
    if mode == "member":
        out.append(f"\ttemplate <class Archive>\n\tvoid serialize(Archive& ar) {{\n\t\t{body('ar', '')};\n\t}}\n")
    out.append("};\n")
    if mode == "free":
        out.append(f"template <class Archive>\nvoid serialize(Archive& ar, S{i}& v) {{\n\t{body('ar', 'v.')};\n}}\n")
    out.append("\n")

out.append("} // namespace user\n\n")

if mode == "spec":
    for i in range(count):
        out.append(f"template <>\nstruct vide::serializer<user::S{i}> {{\n\ttemplate <class Archive>\n\tstatic void serialize(Archive& ar, user::S{i}& v, std::uint32_t) {{\n\t\t{body('ar', 'v.')};\n\t}}\n}};\n\n")

chain_ends = [i for i in range(count) if i % chain_length == chain_length - 1 or i == count - 1]
objects = "".join(f"\tuser::S{i} out{i}, in{i};\n" for i in chain_ends)
saves = "".join(f"\t\toar(out{i});\n" for i in chain_ends)
loads = "".join(f"\t\tiar(in{i});\n" for i in chain_ends)
checks = " && ".join(f"in{i}.a == out{i}.a" for i in chain_ends)

out.append(f"""template <class IArchive, class OArchive>
bool roundtrip() {{
{objects}	std::stringstream ss;
	{{
		OArchive oar(ss);
{saves}	}}
	{{
		IArchive iar(ss);
{loads}	}}
	return {checks};
}}

""")
out.append(f"""int main() {{
	bool ok = roundtrip<vide::BinaryInputArchive, vide::BinaryOutputArchive>();
	ok &= roundtrip<vide::PortableBinaryInputArchive, vide::PortableBinaryOutputArchive>();
	ok &= roundtrip<vide::JSONInputArchive, vide::JSONOutputArchive>();
	ok &= roundtrip<vide::XMLInputArchive, vide::XMLOutputArchive>();
	return ok ? 0 : 1;
}}
""")
print("".join(out))
