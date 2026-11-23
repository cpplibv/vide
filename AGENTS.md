# vide

Header-only C++26 serialization library, forked from [cereal](https://github.com/USCiLab/cereal) and **not compatible** with it.
Turns arbitrary types into binary, portable binary, JSON or XML and back. Adds proxy archives, validators and
reflection based serialization on top of the cereal model. Data format is stable since v3.0.0: changes that alter
serialized output are breaking changes.

## Layout

- `include/vide/` - the library; everything is a header
  - `vide.hpp` - core: `InputArchive` / `OutputArchive`, dispatch to `vide::serializer<T>`
  - `serializer.hpp` - `vide::serializer<T>`, the only serialization entry point: specializations for non-intrusive
    types, the primary template forwards to the member shapes (`serialize`, `save`/`load`, `*_minimal`, each optionally
    versioned). No free-function serializers, no ADL lookup
  - `archives/` - `binary`, `portable_binary`, `json`, `xml`, `proxy_archive`
  - `types/` - serializers for std (`std_*.hpp`), boost flat containers, smart pointers (`memory.hpp`), polymorphism,
    enums, `reflection.hpp`
  - `details/` - internals: traits, validation, polymorphic registry, helpers
  - `macros.hpp` - user-overridable configuration (`VIDE_SIZE_TYPE`, `VIDE_POLYMORPHIC_ID_TYPE`, function names, ...)
  - `external/` - bundled rapidjson and rapidxml; third-party code, leave it alone
- `unittests/` - doctest tests, one executable per `.cpp`, grouped by area: `core/`, `archives/`, `std/`, `memory/`,
  `polymorphic/`, `boost/` (needs boost); `common/` holds the shared test code
- `cmake/CPM.cmake` - CPM bootstrap; fetches doctest when tests are built, and Boost headers if no installed Boost is
  found and vide is the top level project (set `CPM_SOURCE_CACHE` to reuse downloads)
- `sandbox/` - example and experiment programs, also built by default
- `doc/` - Doxygen setup; built only when Doxygen is found
- `README.md` - user docs and the change log

## Build and test

Requires GCC 16+ (`-freflection` is always on), CMake 3.24+, Ninja. Build types: `DEBUG`, `RELEASE`, `DEV`
(`-O3` without static linking; the usual one for development).

```sh
cmake -S . -B build/dev -G Ninja -DCMAKE_BUILD_TYPE=DEV -DCMAKE_LINKER_TYPE=LLD
ninja -C build/dev -j 4
ctest --test-dir build/dev -j 8 --output-on-failure
```

- Always link with LLD (`-DCMAKE_LINKER_TYPE=LLD`)
- One compile peaks around 1.5 GB of RAM; size `-j` to the machine, not the core count. A full build is ~110 targets
- Every public header is compiled standalone (`VERIFY_INTERFACE_HEADER_SETS`), so each header must include what it uses
- Strict warning set (see `CMakeLists.txt`); new code should build warning free
- Must work on both Windows (MinGW) and Linux. Classic trap: `int64_t` is `long` on Linux and `long long` on
  Windows; never overload on fixed-width typedefs, dispatch on `sizeof` and signedness instead

## Tests

- New test file: drop a `.cpp` into the matching `unittests/<area>/`, it is picked up by glob; the target and test are
  named after folder and file (`core/pod.cpp` -> `test_core_pod`); files mirroring a `vide/types/` header keep its name
  (`std/std_vector.cpp` -> `test_std_vector`)
- Include what the test uses: the `vide/types/*` serializers it exercises, and only the `common/` headers it needs
- One `TEST_CASE_TEMPLATE` per feature over an archive list from `common/archives.hpp` (`VIDE_ALL_ARCHIVES`,
  `VIDE_ALL_BINARY_ARCHIVES`, `VIDE_ALL_TEXT_ARCHIVES`, ...), `SUBCASE`s for its aspects:
  ```cpp
  TEST_CASE_TEMPLATE("std::vector", Ar, VIDE_ALL_ARCHIVES) {
  	auto gen = test_rng();
  	check_roundtrip<Ar>(random_container<std::vector<int>>(gen, 100));
  }
  ```
- `check_roundtrip<Ar>(value)` (`common/check_roundtrip.hpp`) is the default assertion: the loaded value equals the
  saved one, and every instrumented object inside ran its own save and load exactly once. Equal values alone can come
  from the wrong serialization function or from a skipped load
- Instrumented: a type whose serialization functions start with `record_call<Archive>(this)`
  (`common/serialization_calls.hpp`); the sample structs are, and a test's own types should be (pass them as
  `check_roundtrip<Ar, MyType>`). Beyond `check_roundtrip`, open a `CallLog` and check `calls.check<T>(n)` after
- Loading starts from a value initialized object: give values that differ from it
- `common/roundtrip.hpp`: `roundtrip` (several values share one archive), `roundtrip_as`, `save`, `load`,
  `load_into`, `saves` / `loads` (validators)
- Randomness only through `test_rng()`: seeded by `--rand-seed` (default 0), so failures reproduce
- Other common headers: `random.hpp` (`random_value`, `random_container`), `sample_structs.hpp` (one struct per
  serialization shape, `for_each_sample_struct`), `for_each_type.hpp`, `tester.hpp` (many values through one archive,
  `expect_*` pointer checks), `shared_ptr_helpers.hpp`
- `polymorphic/test_classes.hpp`: polymorphic class zoo, registered once in `polymorphic/test_classes.cpp`, which every
  `polymorphic/` test links. `Tester::run()` checks that no object ran its own save or load twice (a virtual base
  serialized through `base_class` shows up there) and that loads mirror saves per type
- Common headers are compiled standalone too (`VERIFY_INTERFACE_HEADER_SETS`), so they must include what they use
- Portability (32 bit) tests are disabled

## Code style

Target style for new and touched code; older parts don't follow it yet, so don't copy their look.

- Modern, portable C++26; `constexpr` when able
- Fields at the top of the class, before functions
- 1TBS, braces omitted around single-statement bodies
- Tab indentation; statement continuation lines get two extra tabs, constructor initializer lists only one

```cpp
class Window {
	int width;
	int height;

public:
	constexpr Window(int width, int height) :
		width(width),
		height(height) {}

	constexpr bool contains(int x, int y) const {
		if (x < 0 || y < 0)
			return false;

		return x < width &&
				y < height;
	}
};
```

## Conventions

- LF, UTF-8, final newline (`.editorconfig`)
- `.idea/` and CLion `build/*` folders are the maintainer's IDE state; don't edit, revert or commit them
- Commit messages: a single imperative subject line, no body, no trailing period
  ("Fetch doctest with CPM instead of vendoring it", "Add boost flat_set and flat_map serializers")
- README change log: one short line per user-visible change, written for library users; no implementation details,
  internal refactors are left out
- `KNOWN_ISSUES.md` and `todo.txt` track open problems and plans
