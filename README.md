# vide

**A header-only C++26 serialization library.**

Turn arbitrary C++ types into JSON, XML, compact binary or portable binary and back again.
Fast, hardened against malicious input, easy to extend and free of external dependencies.

![C++26](https://img.shields.io/badge/C%2B%2B-26-blue)
![header-only](https://img.shields.io/badge/header--only-yes-brightgreen)
![version](https://img.shields.io/badge/version-4.0.0--experimental-orange)
![license](https://img.shields.io/badge/license-BSD--3--Clause-lightgrey)

> [!WARNING]
> **v4.0.0 is experimental.** It reworks the serialization entry point and the archive internals.
> The API may still change before v4 is final, and data written by some versioned serializers
> changed (see [Release notes](#release-notes)). The latest non-experimental release is **v3.1.0**.

## Features

- **Header-only, no dependencies** - RapidJSON and RapidXML are bundled
- **Four archive formats** - JSON, XML, binary and endian aware portable binary
- **Standard library coverage** - containers, smart pointers, `optional`, `variant`, `chrono` and more, plus boost flat containers
- **Secure by design** - bounded allocations, read size checks, enum value sets, smart pointer type checks
- **Validators** - `ar.nvp("items", items, ar.notempty, ar.maxsize(64))`
- **Proxy archives** - intercept, rewrite or annotate values anywhere in the serialization tree
- **Reflection** - serialize aggregates without boilerplate using C++26 static reflection
- **Versioning** - per class versions, with helpers to skip or convert data written by older versions
- **Polymorphism** - smart pointers to polymorphic bases, also across shared libraries

## Requirements

- A C++26 compiler with static reflection (GCC 16+ with `-freflection`; the CMake target adds the flag)
- CMake 3.24+ (only for the CMake integration)

## Getting started

```cpp
#include <vide/archives/json.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <iostream>

struct Monster {
	std::string name;
	int health = 0;
	std::vector<std::string> loot;

	template <class Archive> void serialize(Archive& ar) {
		ar.nvp("name", name);
		ar.nvp("health", health);
		ar.nvp("loot", loot);
	}
};

int main() {
	Monster goblin{"Goblin", 30, {"dagger", "coin"}};

	vide::JSONOutputArchive ar(std::cout);
	ar.nvp("monster", goblin);
} // The archive completes its output when destroyed
```

```json
{
    "monster": {
        "name": "Goblin",
        "health": 30,
        "loot": [
            "dagger",
            "coin"
        ]
    }
}
```

Loading is the same call on an input archive:

```cpp
Monster monster;
vide::JSONInputArchive ar(stream);
ar.nvp("monster", monster);
```

Swap in `BinaryOutputArchive` / `BinaryInputArchive` from `vide/archives/binary.hpp` for compact binary
output; the serializers stay the same.

### Adding vide to a project

```cmake
# CPM
CPMAddPackage("gh:cpplibv/vide@4.0.0")

# or as a subdirectory
add_subdirectory(vide)

# or as an installed package
find_package(vide 4 REQUIRED)

target_link_libraries(my_app PRIVATE vide::vide)
```

Being header-only, copying `include/vide` into a project works too; compile with `-freflection`.

## Writing serializers

### Member functions

```cpp
struct Player {
	std::string name;
	int score;

	template <class Archive> void serialize(Archive& ar) {
		ar.nvp("name", name);
		ar.nvp("score", score);
	}
};
```

When saving and loading differ, split it into `save` and `load`:

```cpp
struct Inventory {
	std::vector<Item> items;
	std::size_t totalWeight = 0; // Derived, not stored

	template <class Archive> void save(Archive& ar) const {
		ar.nvp("items", items);
	}

	template <class Archive> void load(Archive& ar) {
		ar.nvp("items", items);
		totalWeight = computeWeight(items);
	}
};
```

Private serializers work too, with `friend class vide::access;`.

### Types you cannot modify

Specialize `vide::serializer<T>`:

```cpp
template <>
struct vide::serializer<Point> {
	template <class Archive>
	static void serialize(Archive& ar, Point& p, std::uint32_t /*version*/) {
		ar.nvp("x", p.x);
		ar.nvp("y", p.y);
	}
};
```

### Versioning

```cpp
struct Config {
	static constexpr std::uint32_t serialize_class_version = 2;

	int threads;
	int timeoutMs;

	template <class Archive> void serialize(Archive& ar, std::uint32_t version) {
		ar.nvp("threads", threads);
		if constexpr (Archive::is_input) {
			if (version < 2) {
				ar.template nvp_ignore<int>("legacyFlag"); // Removed in version 2
				timeoutMs = ar.template nvp_load<int>("timeoutSec") * 1000;
				return;
			}
		}
		ar.nvp("timeoutMs", timeoutMs);
	}
};
```

The version is stored once per type per archive. `serialize_class_version` can also be declared on a
`vide::serializer<T>` specialization.

### Archive API

| Call | Meaning |
|---|---|
| `ar.nvp("name", value)` | Serialize a named value; binary archives skip the name |
| `ar(value)` | Serialize an unnamed value; chainable: `ar(a)(b)(c)` |
| `ar.template base_class<B>(this)` | Serialize a base class |
| `ar.template virtual_base_class<B>(this)` | Serialize a virtual base class, once per object |
| `ar.template load<T>()`, `ar.template nvp_load<T>(name)` | Load and return a value; input archives only |
| `ar.template ignore<T>()`, `ar.template nvp_ignore<T>(name)` | Load and discard a value; no-op when saving |
| `Archive::is_input`, `Archive::is_output` | Direction, for `if constexpr` |

## Archives

| Header | Archives | |
|---|---|---|
| `vide/archives/json.hpp` | `JSONOutputArchive`, `JSONInputArchive` | Human readable (RapidJSON) |
| `vide/archives/xml.hpp` | `XMLOutputArchive`, `XMLInputArchive` | Human readable (RapidXML) |
| `vide/archives/binary.hpp` | `BinaryOutputArchive`, `BinaryInputArchive` | Fastest and most compact; same endianness only |
| `vide/archives/portable_binary.hpp` | `PortableBinaryOutputArchive`, `PortableBinaryInputArchive` | Binary, portable across endianness |
| `vide/archives/proxy_archive.hpp` | `ProxyArchive<CRTP, Ar>` | Base for [proxy archives](#proxy-archives) |

Text archives complete their output on destruction, so keep them in a scope.

## Supported types

Arithmetic types, enums and C arrays work out of the box. Everything else needs its header; nothing
is pulled in implicitly.

| Header in `vide/types/` | Types |
|---|---|
| `std_vector`, `std_array`, `std_deque`, `std_list`, `std_forward_list`, `std_valarray` | Sequence containers |
| `std_map`, `std_set`, `std_unordered_map`, `std_unordered_set` | Associative containers, `multi` variants included |
| `std_queue`, `std_stack` | Container adaptors, `priority_queue` included |
| `std_string`, `std_pair`, `std_tuple`, `std_optional`, `std_variant` | Utilities |
| `std_atomic`, `std_bitset`, `std_chrono`, `std_complex`, `functional` | Other std types |
| `memory` | `unique_ptr`, `shared_ptr`, `weak_ptr`, custom smart pointers |
| `polymorphic` | Smart pointers to polymorphic types |
| `boost_flat_map`, `boost_flat_set` | `boost::container::flat_(multi)map`, `flat_(multi)set` |
| `reflection` | Reflection based serialization |

## Validation and security

vide treats input data as untrusted:

- Allocations are bounded by the size of the input, so a forged size cannot exhaust memory
- Binary `bool` values other than `0` and `1` are rejected
- Shared pointers are checked against the type they were first loaded as, polymorphic or not
- Containers with unique keys reject duplicate keys
- Enums can declare their valid values, checked on both save and load
- Built-in validators check individual values, such as sizes, null pointers or empty containers
- User defined validators can enforce any further rule of the data model

### Validators

```cpp
template <class Archive> void serialize(Archive& ar) {
	ar.nvp("owner", owner, ar.notnull);
	ar.nvp("tags", tags, ar.notempty, ar.maxsize(16));
	ar.nvp("children", children, ar.ranged(ar.notnull));
	ar.nvp("cache", cache, ar.indirect_maxsize(1024));
}
```

| Validator | Passes when |
|---|---|
| `notnull` | `var` converts to `true`; for a range, every element does |
| `notnullrange` | The range itself converts to `true` |
| `notempty` | `!var.empty()` |
| `maxsize(n)`, `minsize(n)` | `var.size() <= n`, `var.size() >= n` |
| `indirect(v)` | `!var \|\| v(*var)` |
| `indirect_maxsize(n)` | `!var \|\| var->size() <= n` |
| `ranged(v)` | `v(item)` for every item |

Validators are available on every archive (`ar.notnull`) and in the `vide` namespace (`vide::notnull`).
They run before saving and after loading, a failure throws `vide::Exception`.

A custom validator is any callable taking the value that throws `vide::Exception` on failure:

```cpp
struct is_odd {
	static void operator()(int value) {
		if (value % 2 == 0)
			throw vide::Exception("Value must be odd");
	}
};

ar.nvp("seed", seed, is_odd{});
```

`ar.verify(condition, message)` throws in place for one-off checks.

### Enum value sets

```cpp
enum class Color { red, green, blue, serialize_end_value }; // Valid: red, green, blue
```

An enum declares its valid values with one of the enumerators `serialize_end_value`,
`serialize_max_value` or `serialize_unbounded`, or with an ADL found free function
`serialize_enum_end_value`, `serialize_enum_max_value`, `serialize_enum_unbounded` or `serialize_enum_verify`.
Define `VIDE_STRICT_ENUM_VALUE_SET=1` to require a value set on every serialized enum.

## Proxy archives

A proxy wraps an archive and can intercept any type at any depth. Serializers see the proxy type, so
they can also read context from it.

```cpp
template <typename Ar>
struct RedactingProxy : vide::ProxyArchive<RedactingProxy<Ar>, Ar> {
	using Base = vide::ProxyArchive<RedactingProxy<Ar>, Ar>;
	using Base::Base;
	using Base::process_as;

	template <class As>
	void process_as(As& as, const Password&) {
		as.process_as(as, std::string("***"));
	}
};

vide::JSONOutputArchive ar(os);
RedactingProxy<vide::JSONOutputArchive> proxy(ar);
proxy.nvp("login", login); // Every Password is written as "***"
```

Polymorphic serialization falls back to the underlying archive. A proxy can opt out of validation with
`static constexpr bool enforce_validation = false;`.

## Reflection

> [!NOTE]
> Reflection support is early and needs a compiler with C++26 static reflection.

```cpp
#include <vide/types/reflection.hpp>

struct Settings {
	using serialize_enable_reflection = void;
	static constexpr std::uint32_t serialize_class_version = 1;

	[[=vide::notnull]] std::shared_ptr<Theme> theme;
	int fontSize = 12;
	std::string language;
};
```

Every non-static data member is serialized by name. Validators attach as annotations.

## Polymorphism

```cpp
#include <vide/types/polymorphic.hpp>

struct Shape {
	virtual ~Shape() = default;
	template <class Archive> void serialize(Archive&) {}
};

struct Circle : Shape {
	float radius;

	template <class Archive> void serialize(Archive& ar) {
		ar.template base_class<Shape>(this);
		ar.nvp("radius", radius);
	}
};

VIDE_REGISTER_TYPE(Circle);
```

A `std::unique_ptr<Shape>` or `std::shared_ptr<Shape>` holding a `Circle` now round trips as a `Circle`.
`VIDE_REGISTER_TYPE_WITH_NAME` sets a stable name; `VIDE_REGISTER_DYNAMIC_INIT` covers types in shared libraries.

## Minimal serialization

A minimal type is stored as one bare value: text archives write `"id": 5` instead of `"id": {"value0": 5}`.

```cpp
struct Id {
	std::int32_t value;

	template <class Archive> std::int32_t& serialize_minimal(Archive&) {
		return value;
	}
};
```

`save_minimal` / `load_minimal` pairs cover asymmetric cases. A `vide::serializer<T>` specialization opts in
with `static constexpr bool minimal = true;`:

```cpp
template <>
struct vide::serializer<Id> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, Id& id, std::uint32_t) {
		ar(id.value); // Exactly one unnamed value
	}
};
```

> [!IMPORTANT]
> A minimal `serialize` must make **exactly one unnamed `ar(value)` call**. No node is opened for a minimal
> type, so a second value, or a named one, ends up in the enclosing object and corrupts its layout.

## Configuration

Define these before including vide, or project wide.

| Macro | Default | |
|---|---|---|
| `VIDE_SIZE_TYPE` | `std::uint32_t` | Type of stored container sizes |
| `VIDE_POLYMORPHIC_ID_TYPE` | `std::uint16_t` | Type of stored polymorphic ids |
| `VIDE_STRICT_ENUM_VALUE_SET` | `0` | Require a value set on every enum |
| `VIDE_DIAGNOSTIC_COLORS` | `0` | Highlight type names in vide's compile errors with ANSI colors; logs without ANSI support show the escapes as text |
| `VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER` | `8` | Reservation budget as a multiple of the input size; beyond it containers grow element wise |
| `VIDE_XML_ROOT_TAG_NAME` | `"root"` | Root element of XML archives |
| `VIDE_CLASS_VERSION_TAG_NAME` | `"vide_class_version"` | Name of stored class versions in text archives |
| `VIDE_FUNCTION_NAME_*` | `serialize`, `save`, ... | Names of the member serialization functions |

CMake options, all on by default when vide is the top level project:
`VIDE_BUILD_TESTS`, `VIDE_BUILD_SANDBOX`, `VIDE_BUILD_DOC`, `VIDE_INSTALL`.

## Relation to cereal

vide started as a fork of [USCiLab/cereal](https://github.com/USCiLab/cereal). It keeps cereal's overall
model, and with it most of cereal's caveats, but it is **not compatible** with cereal: neither the API nor
the data format. The new name makes that explicit.

Compared to cereal, vide:

- fixes a number of security vulnerabilities in deserialization
- adds proxy archives, validators, enum value sets and reflection
- has a single serialization entry point (`vide::serializer<T>`) and a dependent name archive API (`ar.nvp(...)`)
- compiles significantly faster
- targets modern C++ only

Relevant upstream fixes are ported by hand; synced with cereal up to `22a1b369` (2026-07-24).

## Migration guide

### From cereal

1. Rename: `cereal` namespace, `<cereal/...>` includes and `CEREAL_*` macros become `vide`, `<vide/...>` and `VIDE_*`.
   Std type headers gained an `std_` prefix: `types/vector.hpp` -> `types/std_vector.hpp`.
2. Free `serialize` / `save` / `load` functions become `vide::serializer<T>` specializations.
3. `ar(a, b, c)` becomes `ar(a)(b)(c)`; `CEREAL_NVP(x)` becomes `ar.nvp("x", x)`.
4. `CEREAL_CLASS_VERSION(T, n)` becomes `static constexpr std::uint32_t serialize_class_version = n;`.
5. `load_and_construct` is gone; types are default constructed (`vide::access` can reach private constructors).
6. Data written by cereal cannot be read by vide.

### From vide 3.x

- Free serialization functions become `vide::serializer<T>` specializations
- `VIDE_CLASS_VERSION` becomes `serialize_class_version`; versioned serializers without one no longer store a version, so their data has to be re-saved
- Code using polymorphic pointers or `VIDE_REGISTER_TYPE` includes `vide/types/polymorphic.hpp` explicitly
- Custom text archives provide public `setNextName`, `startNode` and `finishNode`

## Roadmap

- Reflection: comprehensive reflection based serialization and metaprogramming
- Reflection: enum value sets through annotations instead of special enumerators
- Detailed error reports with serialization stack and archive position, also for validation
- Visitor / foreach algorithms over serializable types
- Archive wide and scoped versions: `ar.archive_version(...)`, `ar.scope_version(...)`
- Maybe: context variables passed as additional serializer arguments

## Release notes

### 4.0.0 (experimental)

- Breaking: `vide::serializer<T>` is the only serialization entry point; free `serialize`, `save`, `load` and `*_minimal` functions are no longer used. Member functions work as before
- Breaking: Remove `VIDE_CLASS_VERSION`; `serialize_class_version` on the type or on its `vide::serializer<T>` is the only class version
- Breaking: Versioned serializers without `serialize_class_version` no longer store a version, which changes their data
- Breaking: `vide.hpp` and the archive headers no longer include `types/polymorphic.hpp`
- Breaking: Map serialization covers the std and boost map types only, not every type with a `mapped_type`
- Breaking: Text archives provide public `setNextName`, `startNode` and `finishNode`; `nvp()` names values through `setNextName`
- Breaking: In text archives the name of a deferred value or of an already serialized virtual base no longer lands on the next unnamed value
- Breaking: Remove the `THREAD_SAFE` option
- Breaking: Every header requires static reflection (`-freflection`), not only `types/reflection.hpp`
- Add: `static constexpr bool minimal = true` for `vide::serializer<T>` specializations
- Add: boost `flat_multiset` serializer
- Improve: Faster compilation of serializer lookup and polymorphic registration
- Improve: Shallower call stacks, easier to step through in a debugger
- Improve: A missing serializer or an incomplete type is a single compile error that names the type and the archive
- Add: `VIDE_DIAGNOSTIC_COLORS=1` colors the type names in compile errors
- Fix: `ar.ranged()` is available on output archives
- Fix: Possible hash collision between registered class versions
- Fix: `long long` and `unsigned long long` JSON deserialization on Linux
- Fix: Linux build
- Fix: Proxy archives preferring their forwarding `process_as` over a derived proxy's overload for a const type
- Fix: `types/std_queue.hpp` missing the serializer of `std::priority_queue`'s default container
- Fix: Missing RapidJSON headers in the installed package
- Build: `CMAKE_COLOR_DIAGNOSTICS` replaces `VIDE_FORCE_COLORED_OUTPUT`, `CMAKE_LINKER_TYPE` replaces `VIDE_ALTERNATIVE_LINKER`
- Build: `VIDE_SKIP_PERFORMANCE_COMPARISON` becomes the opt-in `VIDE_BUILD_PERFORMANCE_COMPARISON`; new opt-in `VIDE_BUILD_PORTABILITY_TEST`
- Build: Package files install to `share/cmake/vide`; the default build type is only set when vide is the top level project

### 3.1.0

- Breaking: Require C++26 and CMake 3.24
- Breaking: Duration and time_point serialization is minimal
- Breaking: Remove `AllowEmptyClassElision` and pkg-config file generation
- Add: Reflection based serialization in `types/reflection.hpp`, with validator annotations on members
- Add: Validators `indirect`, `indirect_maxsize`, `ranged` and `minsize`
- Fix: Multi-container deserialization order for equal keys
- Build: Improved CMake scripts, stricter warnings, every public header compiles standalone
- Sync: Upstream cereal up to `22a1b369` (2026-07-24)

### 3.0.2

- Breaking: Move `vide/types/base_class.hpp` to `vide/base_class.hpp`
- Fix: Missing `base_class` include

### 3.0.1

- Add: `base_class` / `virtual_base_class` dependent names on proxy archives

### 3.0.0

- Breaking: New `shared_ptr` and polymorphic data representation
- Breaking: No empty node for a repeated virtual base in text archives
- Breaking: Std type serializer headers gain an `std_` prefix
- Security: Shared pointers can no longer be manipulated into pointing at a different type after loading
- Add: Generic smart pointer support
- Add: `serialize_minimal()` as a shorthand for `save_minimal` / `load_minimal`
- Add: `notnullrange` validator, element checks for `notnull`, validators on NVPs
- Add: Duplicate key detection when loading containers with unique keys
- Add: boost `flat_set` and `flat_map` serializers
- Add: `ar.base_class<B>(this)` and `ar.virtual_base_class<B>(this)` dependent names
- Add: `VIDE_POLYMORPHIC_ID_TYPE`, defaults to `std::uint16_t`
- Improve: Reworked polymorphic serialization

### 2.5.0

- Security: Enum value sets, verified on save and load; `VIDE_STRICT_ENUM_VALUE_SET`
- Add: Validators `notnull`, `notempty`, `maxsize` and `ar.verify(condition, message)`
- Add: `ar.ignore<T>()`, `ar.nvp_ignore<T>(name)`, `ar.load<T>()` and `ar.nvp_load<T>(name)`
- Sync: Upstream cereal and several upstream PRs; updated RapidJSON and RapidXML

### 2.4.0

- Breaking: Rename `VIDE_XML_STRING_VALUE` to `VIDE_XML_ROOT_TAG_NAME`
- Breaking: Remove `operator<<`, `operator>>` and `operator&`
- Security: Binary `bool` rejects values other than `0` and `1`
- Add: Static `serialize_class_version` member and `VIDE_CLASS_VERSION_TAG_NAME`

### 2.3.0

- Breaking: Serialized data format changed
- Security: Bounded memory reservation during deserialization: `ar.safe_to_reserve<T>()`, `ar.validate_read_size<T>()`
- Add: Archive traits `is_binary_archive`, `size_type`, `supports_binary<T>`, and `ar.size_tag()`
- Improve: Faster compilation, modernized type traits

### 2.2.x

- Breaking: Remove specialization based disambiguation of duplicate serializers
- Add: `*_minimal` for any type, const reference returns, recursive minimal and rvalue `load_minimal` parameters

### 2.1.0 and earlier

- Fork from cereal and rename to vide
- Breaking: Remove `load_and_construct`, prologue / epilogue functions, multi-value `ar(a, b, c)` and `UserDataAdapter`
- Add: Proxy archives and the `process_as` customization point
- Add: Dependent name API `ar.nvp("name", value)` and the `IgnoreNVP` archive flag
- Improve: Movable archives, archive templates, no more `VIDE_SETUP_ARCHIVE_TRAITS`
- Improve: Finer grained headers: `nvp.hpp`, `access.hpp`, `binary_data.hpp`, `size_tag.hpp`, ...

## Known issues

See [KNOWN_ISSUES.md](KNOWN_ISSUES.md).

## License

vide is licensed under the [BSD 3-Clause license](LICENSE). Bundled third-party code keeps its own license
(see `include/vide/external`).
