**VaderY/vide** - A C++23 library for serialization
==========================================
Based on and forked from: [USCiLab/cereal](https://github.com/USCiLab/cereal)

This is an experimental fork that alters multiple core functionality of [USCiLab/cereal](https://github.com/USCiLab/cereal) and therefore **is not compatible** with the upstream!
Neither forward, nor backward compatibility is guaranteed.  
Bugfixes from the upstream are planned to be ported manually (and currently in sync with 2022.03.27 ddd46724).

### Changes / Differences to Cereal:
- Change name to `vide` to indicate the incompatibility with upstream
  - `vide` comes from latin word serial
  - Name change was necessary due to incompatibilities
- Remove some legacy compiler support
- Bump required versions to C++23, GCC 11.2, CMake 3.20
- Fixes and breaks some minor stuff
- Remove `VIDE_SETUP_ARCHIVE_TRAITS` (Input and output archives are no longer linked)
  - Pro: Enables archives to be template types
  - Pro: One less macro that has to be called
  - Pro: Allows single in or out direction archives or type supports
  - Con: load_minimal type deduction is now done with the input archives on the save_minimal function (never called, only instantiated for type deduction)
  - Con: No check if save_minimal and load_minimal are correctly using the same type
  - Note: Cons could be negated with a single typedef inside the input archive to the output archive
- New archive flag `vide::IgnoreNVP`: Add support for specifying if archives ignores name from NVPs (previously this was hardcoded for the built-in binary archive only)
- Move NVP into its own header
- Move `vide::access` into its own header and add access_fwd.hpp header for forward declaration only
- Move `BinaryData`, `SizeTag`, `MapItem` and `construct` into their own header
- Rework type serializers to only include what is required
- Remove `load_and_construct`
- Remove pointer to derived in archive bases (just use this with CRTP)
- Make archives movable
- Remove the experimental UserDataAdapter (A better solution will come)
- Remove the ability to call the archives with multiple member at the same times as `ar(member0, member1, member2)`. Chaining is still possible prefer that syntax `ar(member0)(member1)(member2)` or just use multiple calls
  - Pro: Enables some future shenanigans 
  - Pro: Alternative syntax are more clear and has same number of character, and has better auto formatting
  - Pro: More clear evaluation order, less variadic template
  - Con: More breakage
- Add proxy archives (A way to inject context information into the serialization chain)
  - NOTE: polymorphic serialization will fall back to the underlying archives and not use the proxy
- Add process_as customization point for archives to handle special types
- Remove prologue and epilogue function support (process_as can take care of it)
- Add `ar.nvp("var", var)` syntax to allow option to not include any header file and really on dependent lookup only
- Bump version to 2.1.0 and start versioning Vide
- Version 2.2.0:
  - Remove specialize/specialization feature that could disambiguate in duplicate serialization methods.  
    (For now serialization methods consistency is required in inheritance hierarchies)
  - Remove string/arithmetic type restriction from `load_minimal`/`save_minimal`
  - Add support for const reference return type during `minimal` serialization
  - Add support for recursive `minimal` serialization (But it is recommended to only use it with primitive/trivial types)


### Planned:
- An alternative for `load_minimal`/`save_minimal` syntax with `T& serialize_transparent()` which does them in a single step
- Foreach iteration algorithms
- Add safe/unsafe data serialization support
  - For unsafe data every reserve / resize is disabled (and/or limited by the archive's max size)
  - `ar.limit(100)(var)` -> If SizeTag exceeds the limit fail with exception
- Improved compile time performance (by organizing includes)
- Improved compile time performance (by improving meta programming practices)
- Improved compile time performance (general)
- Scoped versions and version guards: `const auto version_guard = ar.scope_version(config_version);` and `ar.scope_version()`
- Maybe: Context variables passed as additional function arguments
- Maybe: Versioned<->type selector

-------------------------------------------------------------------------------------------------

cereal - A C++11 library for serialization
==========================================

<p>cereal is a header-only C++11 serialization library.  cereal takes arbitrary data types and reversibly turns them into different representations, such as compact binary encodings, XML, or JSON.  cereal was designed to be fast, light-weight, and easy to extend - it has no external dependencies and can be easily bundled with other code or used standalone.</p>

### cereal has great documentation

Looking for more information on how cereal works and its documentation?  Visit [cereal's web page](https://USCiLab.github.io/cereal) to get the latest information.

### cereal is easy to use

Installation and use of of cereal is fully documented on the [main web page](https://USCiLab.github.io/cereal), but this is a quick and dirty version:

* Download cereal and place the headers somewhere your code can see them
* Write serialization functions for your custom types or use the built in support for the standard library cereal provides
* Use the serialization archives to load and save data

```cpp
#include <vide/types/unordered_map.hpp>
#include <vide/types/memory.hpp>
#include <vide/archives/binary.hpp>
#include <fstream>

struct MyRecord {
	uint8_t x, y;
	float z;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(z);
	}
};

struct SomeData {
	int32_t id;
	std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;

	template <class Archive>
	void save(Archive& ar) const {
		ar(data);
	}

	template <class Archive>
	void load(Archive& ar) {
		static int32_t idGen = 0;
		id = idGen++;
		ar(data);
	}
};

int main() {
	std::ofstream os("out.bin", std::ios::binary | std::ios::out);

	SomeData myData;
	{
		vide::BinaryOutputArchive archive(os);
		archive(myData);
	}

	return 0;
}
```    

### cereal has a mailing list

Either get in touch over <a href="mailto:cerealcpp@googlegroups.com">email</a> or [on the web](https://groups.google.com/forum/#!forum/cerealcpp).



## cereal has a permissive license

cereal is licensed under the [BSD license](http://opensource.org/licenses/BSD-3-Clause).

## cereal build status

* master : [![Build Status](https://travis-ci.com/USCiLab/cereal.svg?branch=master)](https://travis-ci.com/USCiLab/cereal)
[![Build status](https://ci.appveyor.com/api/projects/status/91aou6smj36or0vb/branch/master?svg=true)](https://ci.appveyor.com/project/AzothAmmo/cereal/branch/master)

---

Were you looking for the Haskell cereal?  Go <a href="https://github.com/GaloisInc/cereal">here</a>.
