**VaderY/cereal** - Fork

This fork is experimental and wishes to alter multiple core functionality of the library and therefore no longer compatible with the upstream!

- Remove some legacy compiler support
- Bump required versions to C++23, GCC 11.2, CMake 3.20
- Fixes and breaks some minor stuff
- Remove `CEREAL_SETUP_ARCHIVE_TRAITS` (Input and output archives are no longer linked)
  - Pro: Enables archives to be template types
  - Pro: One less macro that has to be called
  - Pro: Allows single in or out direction archives or type supports
  - Con: load_minimal type deduction is now done with the input archives on the save_minimal function (never called, only instantiated for type deduction)
  - Con: No check if save_minimal and load_minimal are correctly using the same type
  - Note: Cons could be negated with a single typedef inside the input archive to the output archive
- New archive flag `cereal::IgnoreNVP`: Add support for specifying if archives ignores name from NVPs (previously this was hardcoded for the built-in binary archive only)
- Move NVP into its own header
- Move `cereal::access` into its own header and add access_fwd.hpp header for forward declaration only
- Move `BinaryData`, `SizeTag`, `MapItem` and `constrcut` into their own header
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


Planned:
- Add safe/unsafe data serialization support
  - For unsafe data every reserve / resize is disabled (and/or limited by the archive's max size)
- Improved compile time performance
- Maybe: Context variables passed as additional function arguments
- Change name to cerealv or cev to indicate the incompatibility with upstream


cereal - A C++11 library for serialization
==========================================

<img src="https://uscilab.github.io/cereal/assets/img/cerealboxside.png" align="right"/><p>cereal is a header-only C++11 serialization library.  cereal takes arbitrary data types and reversibly turns them into different representations, such as compact binary encodings, XML, or JSON.  cereal was designed to be fast, light-weight, and easy to extend - it has no external dependencies and can be easily bundled with other code or used standalone.</p>

### cereal has great documentation

Looking for more information on how cereal works and its documentation?  Visit [cereal's web page](https://USCiLab.github.io/cereal) to get the latest information.

### cereal is easy to use

Installation and use of of cereal is fully documented on the [main web page](https://USCiLab.github.io/cereal), but this is a quick and dirty version:

* Download cereal and place the headers somewhere your code can see them
* Write serialization functions for your custom types or use the built in support for the standard library cereal provides
* Use the serialization archives to load and save data

```cpp
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
    
struct MyRecord
{
  uint8_t x, y;
  float z;
  
  template <class Archive>
  void serialize( Archive & ar )
  {
    ar( x, y, z );
  }
};
    
struct SomeData
{
  int32_t id;
  std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;
  
  template <class Archive>
  void save( Archive & ar ) const
  {
    ar( data );
  }
      
  template <class Archive>
  void load( Archive & ar )
  {
    static int32_t idGen = 0;
    id = idGen++;
    ar( data );
  }
};

int main()
{
  std::ofstream os("out.cereal", std::ios::binary);
  cereal::BinaryOutputArchive archive( os );

  SomeData myData;
  archive( myData );

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
