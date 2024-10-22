/*! \file vide.hpp
    \brief Main vide functionality */
/*
  Copyright (c) 2013-2022, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vide/access.hpp>
#include <vide/binary_data.hpp>
#include <vide/details/bits.hpp>
#include <vide/details/helpers.hpp>
#include <vide/details/traits.hpp>
#include <vide/macros.hpp>
#include <vide/nvp.hpp>
#include <vide/size_tag.hpp>
#include <vide/types/base_class.hpp>
#include <vide/unserializable_type_tag.hpp>


namespace vide {

// ######################################################################
//! Marks data for deferred serialization
/*! vide performs a recursive depth-first traversal of data it serializes. When
	serializing smart pointers to large, nested, or cyclical data structures, it
	is possible to encounter a stack overflow from excessive recursion when following
	a chain of pointers.

	Deferment can help in these situations if the data can be serialized separately from
	the pointers used to traverse the structure. For example, a graph structure can have its
	nodes serialized before its edges:

	@code{.cpp}
	struct MyEdge
	{
	  std::shared_ptr<MyNode> connection;
	  int some_value;

	  template<class Archive>
	  void serialize(Archive & archive)
	  {
		// when we serialize an edge, we'll defer serializing the associated node
		archive( vide::defer( connection ),
				 some_value );
	  }
	};

	struct MyGraphStructure
	{
	  std::vector<MyEdge> edges;
	  std::vector<MyNodes> nodes;

	  template<class Archive>
	  void serialize(Archive & archive)
	  {
		// because of the deferment, we ensure all nodes are fully serialized
		// before any connection pointers to those nodes are serialized
		archive( edges, nodes );

		// we have to explicitly inform the archive when it is safe to serialize
		// the deferred data
		archive.serializeDeferments();
	  }
	};
	@endcode

	@relates DeferredData
	@ingroup Utility */
template <class T>
inline DeferredData<T> defer(T&& value) {
	return DeferredData<T>{std::forward<T>(value)};
}

// ######################################################################
//! Special flags for archives
/*! AllowEmptyClassElision
	  This allows for empty classes to be serialized even if they do not provide
	  a serialization function.  Classes with no data members are considered to be
	  empty.  Be warned that if this is enabled and you attempt to serialize an
	  empty class with improperly formed serialize or load/save functions, no
	  static error will occur - the error will propagate silently and your
	  intended serialization functions may not be called.  You can manually
	  ensure that your classes that have custom serialization are correct
	  by using the traits is_output_serializable and is_input_serializable
	  in vide/details/traits.hpp.
	@ingroup Internal */
enum Flags {
	AllowEmptyClassElision = 1u << 0u,
	IgnoreNVP = 1u << 1u,
	TextArchive = 1u << 2u,
	BinaryArchive = 1u << 3u,
	//	____ = 1u << 3u,
	//	____ = 1u << 4u,
	//	____ = 1u << 5u,
};

// ######################################################################
//! Registers a specific Archive type with vide
/*! This registration should be done once per archive.  A good place to
	put this is immediately following the definition of your archive.
	Archive registration is only strictly necessary if you wish to
	support pointers to polymorphic data types.  All archives that
	come with vide are already registered.
	@ingroup Internal */
#define VIDE_REGISTER_ARCHIVE(Archive)                                \
  namespace vide { namespace detail {                                 \
  template <class T, class BindingTag>                                  \
  typename polymorphic_serialization_support<Archive, T>::type          \
  instantiate_polymorphic_binding( T*, Archive*, BindingTag, adl_tag ); \
  } } /* end namespaces */

//! Helper macro to omit unused warning
#if defined(__GNUC__)
// GCC / clang don't want the function
	#define VIDE_UNUSED_FUNCTION
#else
	#define VIDE_UNUSED_FUNCTION static void unused() { (void)version; }
#endif

// ######################################################################
//! Defines a class version for some type
/*! Versioning information is optional and adds some small amount of
	overhead to serialization.  This overhead will occur both in terms of
	space in the archive (the version information for each class will be
	stored exactly once) as well as runtime (versioned serialization functions
	must check to see if they need to load or store version information).

	Versioning is useful if you plan on fundamentally changing the way some
	type is serialized in the future.  Versioned serialization functions
	cannot be used to load non-versioned data.

	By default, all types have an assumed version value of zero.  By
	using this macro, you may change the version number associated with
	some type.  vide will then use this value as a second parameter
	to your serialization functions.

	The interface for the serialization functions is nearly identical
	to non-versioned serialization with the addition of a second parameter,
	const std::uint32_t version, which will be supplied with the correct
	version number.  Serializing the version number on a save happens
	automatically.

	Versioning cannot be mixed with non-versioned serialization functions.
	Having both types will result result in a compile time error.  Data
	serialized without versioning cannot be loaded by a serialization
	function with added versioning support.

	Example interface for versioning on a non-member serialize function:

	@code{cpp}
	VIDE_CLASS_VERSION( Mytype, 77 ); // register class version

	template <class Archive>
	void serialize( Archive & ar, Mytype & t, const std::uint32_t version )
	{
	  // When performing a load, the version associated with the class
	  // is whatever it was when that data was originally serialized
	  //
	  // When we save, we'll use the version that is defined in the macro

	  if( version >= some_number )
		// do this
	  else
		// do that
	}
	@endcode

	Interfaces for other forms of serialization functions is similar.  This
	macro should be placed at global scope.
	@ingroup Utility */

//! On C++17, define the StaticObject as inline to merge the definitions across TUs
//! This prevents multiple definition errors when this macro appears in a header file
//! included in multiple TUs.
#define VIDE_CLASS_VERSION(TYPE, VERSION_NUMBER)                               \
  namespace vide { namespace detail {                                          \
    template <> struct Version<TYPE>                                             \
    {                                                                            \
      static std::uint32_t registerVersion()                                     \
      {                                                                          \
        ::vide::detail::StaticObject<Versions>::getInstance().mapping.emplace( \
             std::type_index(typeid(TYPE)).hash_code(), VERSION_NUMBER );        \
        return VERSION_NUMBER;                                                   \
      }                                                                          \
      static inline const std::uint32_t version = registerVersion();             \
      VIDE_UNUSED_FUNCTION                                                     \
    }; /* end Version */                                                         \
  } } // end namespaces

// -------------------------------------------------------------------------------------------------
// Base Class Support

namespace detail {

struct base_class_id {
	std::type_index type;
	const void* ptr;
	size_t hash;

	template <class T>
	base_class_id(T const* const t) :
		type(typeid(T)),
		ptr(t),
		hash(std::hash<std::type_index>()(typeid(T)) ^ (std::hash<const void*>()(t) << 1)) {
	}

	bool operator==(base_class_id const& other) const {
		return type == other.type && ptr == other.ptr;
	}
};

struct base_class_id_hash {
	size_t operator()(base_class_id const& id) const {
		return id.hash;
	}
};

} // namespace detail

// -------------------------------------------------------------------------------------------------

// ######################################################################
//! The base output archive class
/*! This is the base output archive for all output archives.  If you create
	a custom archive class, it should derive from this, passing itself as
	a template parameter for the ArchiveType.

	The base class provides all of the functionality necessary to
	properly forward data to the correct serialization functions.

	Individual archives should use a combination of prologue and
	epilogue functions together with specializations of serialize, save,
	and load to alter the functionality of their serialization.

	@tparam ArchiveType The archive type that derives from OutputArchive (CRTP)
	@tparam Flags Flags to control advanced functionality.  See the Flags
				  enum for more information.
	@ingroup Internal */
template <class ArchiveType, std::uint32_t Flags = 0>
class OutputArchive : public detail::OutputArchiveBase {
public:
	static constexpr bool ignores_nvp = (Flags & vide::IgnoreNVP) != 0;
	static constexpr bool is_proxy = false;
	static constexpr bool is_output = true;
	static constexpr bool is_input = false;
	static constexpr bool is_text_archive = (Flags & vide::TextArchive) != 0;
	static constexpr bool is_binary_archive = (Flags & vide::BinaryArchive) != 0;

	template <typename T>
	static constexpr bool supports_binary = binary_serializable_type<T> && is_binary_archive;

public:
	using size_type = vide::size_type;

private:
	//! A set of all base classes that have been serialized
	std::unordered_set<detail::base_class_id, detail::base_class_id_hash> itsBaseClassSet;

	//! Maps from addresses to pointer ids
	std::unordered_map<const void*, std::uint32_t> itsSharedPointerMap;

	//! Copy of shared pointers used in #itsSharedPointerMap to make sure they are kept alive
	//  during lifetime of itsSharedPointerMap to prevent CVE-2020-11105.
	std::vector<std::shared_ptr<const void>> itsSharedPointerStorage;

	//! The id to be given to the next pointer
	std::uint32_t itsCurrentPointerId = 1;

	//! Maps from polymorphic type name strings to ids
	std::unordered_map<const char*, std::uint32_t> itsPolymorphicTypeMap;

	//! The id to be given to the next polymorphic type name
	std::uint32_t itsCurrentPolymorphicTypeId = 1;

	//! Keeps track of classes that have versioning information associated with them
	std::unordered_set<std::uint64_t> itsVersionedTypes;

//	int64_t scope_version_ = 0;

	//! Deferments
	std::vector<std::function<void(void)>> itsDeferments;

public:
	//! Construct the output archive
	inline OutputArchive() = default;
	OutputArchive(const OutputArchive&) = delete;
	OutputArchive& operator=(const OutputArchive&) & = delete;
	inline OutputArchive(OutputArchive&&) noexcept = default;
	inline OutputArchive& operator=(OutputArchive&&) & noexcept = default;

private:
	inline ArchiveType& self() {
		return *static_cast<ArchiveType*>(this);
	}

	inline const ArchiveType& self() const {
		return *static_cast<const ArchiveType*>(this);
	}

public:
	//! Serializes the passed in data
	/*! This is the primary interface for serializing data with an archive */
	template <class T>
	inline ArchiveType& operator()(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	template <typename T>
	inline ArchiveType& nvp(const char* name, T&& arg) {
		if constexpr (ignores_nvp)
			return (*this)(std::forward<T>(arg));
		else
			return (*this)(::vide::make_nvp(name, std::forward<T>(arg)));
	}

	inline ArchiveType& size_tag(uint32_t size) {
		return (*this)(::vide::SizeTag<size_type>(size));
	}

	inline ArchiveType& size_tag(uint64_t size) {
		return (*this)(::vide::SizeTag<size_type>(size));
	}

	// inline ArchiveType& binary_data(uint64_t size) {
	// 	return (*this)(::vide::SizeTag<size_type>(size));
	// }

	/*! @name Boost Transition Layer
		Functionality that mirrors the syntax for Boost.  This is useful if you are transitioning
		a large project from Boost to vide.  The preferred interface for vide is using operator(). */
	//! @{

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		vide.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator&(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		vide.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator<<(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	//! @}

public:
	//! Serializes any data marked for deferment using defer
	/*! This will cause any data wrapped in DeferredData to be immediately serialized */
	void serializeDeferments() {
		for (auto& deferment : itsDeferments)
			deferment();
	}

	//! Registers a shared pointer with the archive
	/*! This function is used to track shared pointer targets to prevent
		unnecessary saves from taking place if multiple shared pointers
		point to the same data.

		@internal
		@param sharedPointer The shared pointer itself (the address is taken via get()).
							 The archive takes a copy to prevent the memory location to be freed
							 as long as the address is used as id. This is needed to prevent CVE-2020-11105.
		@return A key that uniquely identifies the pointer */
	inline std::uint32_t registerSharedPointer(const std::shared_ptr<const void>& sharedPointer) {
		const void* addr = sharedPointer.get();

		// Handle null pointers by just returning 0
		if (addr == nullptr)
			return 0;
		itsSharedPointerStorage.push_back(sharedPointer);

		auto id = itsSharedPointerMap.find(addr);
		if (id == itsSharedPointerMap.end()) {
			auto ptrId = itsCurrentPointerId++;
			itsSharedPointerMap.insert({addr, ptrId});
			return ptrId | detail::msb_32bit; // mask MSB to be 1
		} else
			return id->second;
	}

	//! Registers a polymorphic type name with the archive
	/*! This function is used to track polymorphic types to prevent
		unnecessary saves of identifying strings used by the polymorphic
		support functionality.

		@internal
		@param name The name to associate with a polymorphic type
		@return A key that uniquely identifies the polymorphic type name */
	inline std::uint32_t registerPolymorphicType(const char* name) {
		auto id = itsPolymorphicTypeMap.find(name);
		if (id == itsPolymorphicTypeMap.end()) {
			auto polyId = itsCurrentPolymorphicTypeId++;
			itsPolymorphicTypeMap.insert({name, polyId});
			return polyId | detail::msb_32bit; // mask MSB to be 1
		} else
			return id->second;
	}

	/// Registers the serialize_class_version static member with the archive and serializes it if necessary.
	/// If this is the first time this class has been serialized, it will record its
	/// serialize_class_version static member and serialize that regardless if the serializer function uses it or not.
	template <class T>
	inline void registerClassVersionUnused() {
		if constexpr (access::has_static_member_class_version<T>) {
			static const auto hash = std::type_index(typeid(T)).hash_code();
			const auto insertResult = itsVersionedTypes.insert(hash);
			std::uint32_t version = access::static_member_class_version<T>();

			if (insertResult.second) // insertion took place, serialize the version number
				process_self(make_nvp<ArchiveType>(VIDE_CLASS_VERSION_TAG_NAME, version));
		}
	}

	//! Registers a class version with the archive and serializes it if necessary
	/*! If this is the first time this class has been serialized, we will record its
		version number and serialize that.

		@tparam T The type of the class being serialized */
	template <class T>
	inline std::uint32_t registerClassVersion() {
		static const auto hash = std::type_index(typeid(T)).hash_code();
		const auto insertResult = itsVersionedTypes.insert(hash);
		std::uint32_t version;
		if constexpr (access::has_static_member_class_version<T>) {
			version = access::static_member_class_version<T>();
		} else {
			const auto lock = detail::StaticObject<detail::Versions>::lock();
			version = detail::StaticObject<detail::Versions>::getInstance().find(hash, detail::Version<T>::version);
		}

		if (insertResult.second) // insertion took place, serialize the version number
			process_self(make_nvp<ArchiveType>(VIDE_CLASS_VERSION_TAG_NAME, version));

		return version;
	}

protected:
	void reset() {
		itsBaseClassSet.clear();
		itsSharedPointerMap.clear();
		itsSharedPointerStorage.clear();
		itsCurrentPointerId = 1;
		itsPolymorphicTypeMap.clear();
		itsCurrentPolymorphicTypeId = 1;
		itsVersionedTypes.clear();
		itsDeferments.clear();
	}

public:
	//! Alternative process function to use a different wrapper type for hierarchy traversal
	template <class As, class T>
	inline void process_as(As& as, const T& var) {
		if constexpr (std::is_same_v<decltype(self().processImpl(as, var)), unserializable_type_tag>)
			self().processImpl(self(), var);
		else
			self().processImpl(as, var);
	}

private:
	//! Serializes data
	template <class T>
	inline void process_self(T&& var) {
		self().process_as(self(), var);
	}

	template <class As, class T>
	inline void processImpl(As& as, const base_class<T>& b) {
		self().processImpl(as, *b.base_ptr);
	}

	template <class As, class T>
	inline void processImpl(As& as, const virtual_base_class<T>& b) {
		detail::base_class_id id(b.base_ptr);
		if (itsBaseClassSet.emplace(id).second)
			self().processImpl(as, *b.base_ptr);
	}

	template <class As, class T>
	inline void processImpl(As& as, DeferredData<T> const& d) {
		(void) as;
		std::function<void(void)> deferment([this, d]() { process_self(d.value); });
		itsDeferments.emplace_back(std::move(deferment));
	}

	//! Empty class specialization
	template <class As, class T>
			requires (!access::is_output_serializable<As, T> && (Flags & AllowEmptyClassElision) != 0 && std::is_empty_v<T>)
	inline void processImpl(As& as, const T& var) {
		(void) as;
		(void) var;
	}

	//! Generic serialization case
	template <class As, class T>
			requires access::is_output_serializable<As, T>
	inline void processImpl(As& as, const T& var) {
		if constexpr (access::has_member_serialize<As, T>) {
			registerClassVersionUnused<T>();
			access::member_serialize(as, const_cast<T&>(var));

		} else if constexpr (access::has_global_serialize<As, T>) {
			registerClassVersionUnused<T>();
			VIDE_FUNCTION_NAME_SERIALIZE(as, const_cast<T&>(var));

		} else if constexpr (access::has_member_save<As, T>) {
			registerClassVersionUnused<T>();
			access::member_save(as, var);

		} else if constexpr (access::has_global_save<As, T>) {
			registerClassVersionUnused<T>();
			VIDE_FUNCTION_NAME_SAVE(as, var);

		} else if constexpr (access::has_member_save_minimal<As, T>) {
			registerClassVersionUnused<T>();
			self().process_as(as, access::member_save_minimal(as, var));

		} else if constexpr (access::has_global_save_minimal<As, T>) {
			registerClassVersionUnused<T>();
			self().process_as(as, VIDE_FUNCTION_NAME_SAVE_MINIMAL(as, var));

		} else if constexpr (access::has_member_serialize_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			access::member_serialize(as, const_cast<T&>(var), version);

		} else if constexpr (access::has_global_serialize_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			VIDE_FUNCTION_NAME_SERIALIZE(as, const_cast<T&>(var), version);

		} else if constexpr (access::has_member_save_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			access::member_save(as, var, version);

		} else if constexpr (access::has_global_save_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			VIDE_FUNCTION_NAME_SAVE(as, var, version);

		} else if constexpr (access::has_member_save_minimal_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			self().process_as(as, access::member_save_minimal(as, var, version));

		} else if constexpr (access::has_global_save_minimal_versioned<As, T>) {
			const auto version = registerClassVersion<T>();
			self().process_as(as, VIDE_FUNCTION_NAME_SAVE_MINIMAL(as, var, version));
		}
	}

	//! No matching serialization
	/*! Invalid if we have invalid output versioning or
		we are not output serializable, and either
		don't allow empty class ellision or allow it but are not serializing an empty class */
	template <class As, class T>
		requires (!access::is_output_serializable<As, T>)
	inline unserializable_type_tag processImpl(As& as, const T&) {
		(void) as;

		static_assert(access::count_output_serializers<ArchiveType, T> != 0, "\n"
				"Vide could not find any output serialization functions for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Serialize functions generally have the following signature:\n\n"
				"  template<class Archive>\n"
				"  void serialize(Archive& ar) {\n"
				"    ar(member1);\n"
				"    ar(member2);\n"
				"  }\n\n");

		static_assert(access::count_output_serializers<ArchiveType, T> <= 1, "\n"
				"Vide found more than one compatible output serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Use specialization (see access.hpp) if you need to disambiguate between serialize vs load/save functions. \n"
				"Note that serialization functions can be inherited which may lead to the aforementioned ambiguities.\n"
				"In addition, you may not mix versioned with non-versioned serialization functions.\n\n");

		return unserializable_type_tag{};
	}
}; // class OutputArchive

// ######################################################################
//! The base input archive class
/*! This is the base input archive for all input archives.  If you create
	a custom archive class, it should derive from this, passing itself as
	a template parameter for the ArchiveType.

	The base class provides all of the functionality necessary to
	properly forward data to the correct serialization functions.

	Individual archives should use a combination of prologue and
	epilogue functions together with specializations of serialize, save,
	and load to alter the functionality of their serialization.

	@tparam ArchiveType The archive type that derives from InputArchive (CRTP)
	@tparam Flags Flags to control advanced functionality.  See the Flags
				  enum for more information.
	@ingroup Internal */
template <class ArchiveType, std::uint32_t Flags = 0>
class InputArchive : public detail::InputArchiveBase {
public:
	static constexpr bool ignores_nvp = (Flags & vide::IgnoreNVP) != 0;
	static constexpr bool is_proxy = false;
	static constexpr bool is_output = false;
	static constexpr bool is_input = true;
	static constexpr bool is_text_archive = (Flags & vide::TextArchive) != 0;
	static constexpr bool is_binary_archive = (Flags & vide::BinaryArchive) != 0;

	template <typename T>
	static constexpr bool supports_binary = binary_serializable_type<T> && is_binary_archive;

public:
	using size_type = vide::size_type;

private:
	//! A set of all base classes that have been serialized
	std::unordered_set<detail::base_class_id, detail::base_class_id_hash> itsBaseClassSet;

	//! Maps from pointer ids to metadata
	std::unordered_map<std::uint32_t, std::shared_ptr<void>> itsSharedPointerMap;

	//! Maps from name ids to names
	std::unordered_map<std::uint32_t, std::string> itsPolymorphicTypeMap;

	//! Maps from type hash codes to version numbers
	std::unordered_map<std::uint64_t, std::uint32_t> itsVersionedTypes;

	//! Deferments
	std::vector<std::function<void(void)>> itsDeferments;

protected:
	/// Stores the amount of bytes that can be resaonably safely reserved during deserialization.
	/// This is a security requirement as a single malicious size_tag could allocage all of system memory.
	/// To improve performance this budget can be used.
	/// Each archive has to initialize this: recommended value is 2-8x times the original raw data size.
	std::size_t reserveMemoryBudget = 64 * 1024; // Defaults to 64 KB but archives are expected to override it.

public:
	//! Construct the output archive
	inline InputArchive() = default;
	InputArchive(const InputArchive&) = delete;
	InputArchive& operator=(const InputArchive&) & = delete;
	inline InputArchive(InputArchive&&) noexcept = default;
	inline InputArchive& operator=(InputArchive&&) & noexcept = default;

private:
	inline ArchiveType& self() {
		return *static_cast<ArchiveType*>(this);
	}

	inline const ArchiveType& self() const {
		return *static_cast<const ArchiveType*>(this);
	}

public:
	//! Serializes the passed in data
	/*! This is the primary interface for serializing data with an archive */
	template <class T>
	inline ArchiveType& operator()(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	template <typename T>
	inline ArchiveType& nvp(const char* name, T&& arg) {
		if constexpr (ignores_nvp)
			return (*this)(std::forward<T>(arg));
		else
			return (*this)(::vide::make_nvp(name, std::forward<T>(arg)));
	}

	inline ArchiveType& size_tag(size_type& size) {
		return (*this)(::vide::SizeTag<size_type&>(size));
	}

	inline size_type size_tag() {
		size_type size;
		size_tag(size);
		return size;
	}

	template <typename T>
	[[nodiscard]] std::size_t safe_to_reserve(size_type numElements) {
		const auto requestedBytes = numElements * sizeof(T);
		const auto numSafeElements = requestedBytes <= reserveMemoryBudget ? numElements : reserveMemoryBudget / sizeof(T);
		reserveMemoryBudget -= numSafeElements * sizeof(T);
		return numSafeElements;
	}

	template <typename T> requires is_binary_archive
	inline void validate_read_size(size_type numElements) {
		const auto requestedBytes = numElements * sizeof(T);
		if (requestedBytes > self().maximumBinaryReadSize())
			throw Exception("Read size validation of " + std::to_string(requestedBytes) + " bytes failed.");
	}

	/*! @name Boost Transition Layer
		Functionality that mirrors the syntax for Boost.  This is useful if you are transitioning
		a large project from Boost to vide.  The preferred interface for vide is using operator(). */
	//! @{

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		vide.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator&(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		vide.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator>>(T&& arg) {
		process_self(std::forward<T>(arg));
		return self();
	}

	//! @}

public:
	//! Serializes any data marked for deferment using defer
	/*! This will cause any data wrapped in DeferredData to be immediately serialized */
	void serializeDeferments() {
		for (auto& deferment : itsDeferments)
			deferment();
	}

	//! Retrieves a shared pointer given a unique key for it
	/*! This is used to retrieve a previously registered shared_ptr
		which has already been loaded.

		@internal
		@param id The unique id that was serialized for the pointer
		@return A shared pointer to the data
		@throw Exception if the id does not exist */
	inline std::shared_ptr<void> getSharedPointer(const std::uint32_t id) {
		if (id == 0)
			return std::shared_ptr<void>(nullptr);

		auto iter = itsSharedPointerMap.find(id);
		if (iter == itsSharedPointerMap.end())
			throw Exception("Error while trying to deserialize a smart pointer. Could not find id " + std::to_string(id));

		return iter->second;
	}

	//! Registers a shared pointer to its unique identifier
	/*! After a shared pointer has been allocated for the first time, it should
		be registered with its loaded id for future references to it.

		@internal
		@param id The unique identifier for the shared pointer
		@param ptr The actual shared pointer */
	inline void registerSharedPointer(const std::uint32_t id, std::shared_ptr<void> ptr) {
		const std::uint32_t stripped_id = id & ~detail::msb_32bit;
		itsSharedPointerMap[stripped_id] = ptr;
	}

	//! Retrieves the string for a polymorphic type given a unique key for it
	/*! This is used to retrieve a string previously registered during
		a polymorphic load.

		@internal
		@param id The unique id that was serialized for the polymorphic type
		@return The string identifier for the type */
	inline std::string getPolymorphicName(const std::uint32_t id) {
		auto name = itsPolymorphicTypeMap.find(id);
		if (name == itsPolymorphicTypeMap.end()) {
			throw Exception("Error while trying to deserialize a polymorphic pointer. Could not find type id " + std::to_string(id));
		}
		return name->second;
	}

	//! Registers a polymorphic name string to its unique identifier
	/*! After a polymorphic type has been loaded for the first time, it should
		be registered with its loaded id for future references to it.

		@internal
		@param id The unique identifier for the polymorphic type
		@param name The name associated with the type */
	inline void registerPolymorphicName(const std::uint32_t id, const std::string& name) {
		const std::uint32_t stripped_id = id & ~detail::msb_32bit;
		itsPolymorphicTypeMap.insert({stripped_id, name});
	}

	/// Registers the serialize_class_version static member with the archive and serializes it if necessary.
	/// If this is the first time this class has been serialized, it will record its
	/// serialize_class_version static member and serialize that regardless if the serializer function uses it or not.
	template <class T>
	inline void loadClassVersionUnused() {
		if constexpr (access::has_static_member_class_version<T>) {
			static const auto hash = std::type_index(typeid(T)).hash_code();
			auto lookupResult = itsVersionedTypes.find(hash);
			if (lookupResult != itsVersionedTypes.end()) // Already loaded
				return;

			// Need to load
			std::uint32_t version;
			process_self(make_nvp<ArchiveType>(VIDE_CLASS_VERSION_TAG_NAME, version));
			itsVersionedTypes.emplace_hint(lookupResult, hash, version);
		}
	}

	//! Registers a class version with the archive and serializes it if necessary
	/*! If this is the first time this class has been serialized, we will record its
		version number and serialize that.

		@tparam T The type of the class being serialized */
	template <class T>
	inline std::uint32_t loadClassVersion() {
		static const auto hash = std::type_index(typeid(T)).hash_code();
		auto lookupResult = itsVersionedTypes.find(hash);

		if (lookupResult != itsVersionedTypes.end()) // already exists
			return lookupResult->second;
		else { // need to load
			std::uint32_t version;

			process_self(make_nvp<ArchiveType>(VIDE_CLASS_VERSION_TAG_NAME, version));
			itsVersionedTypes.emplace_hint(lookupResult, hash, version);

			return version;
		}
	}

protected:
	void reset() {
		itsBaseClassSet.clear();
		itsSharedPointerMap.clear();
		itsPolymorphicTypeMap.clear();
		itsVersionedTypes.clear();
		itsDeferments.clear();
		reserveMemoryBudget = 64 * 1024;
	}

public:
	//! Alternative process function to use a different wrapper type for hierarchy traversal
	template <class As, class T>
	inline void process_as(As& as, T& var) {
		if constexpr (std::is_same_v<decltype(self().processImpl(as, var)), unserializable_type_tag>)
			self().processImpl(self(), var);
		else
			self().processImpl(as, var);
	}

private:
	//! Serializes data
	template <class T>
	inline void process_self(T&& var) {
		self().process_as(self(), var);
	}

	template <class As, class T>
	inline void processImpl(As& as, base_class<T>& b) {
		self().processImpl(as, *b.base_ptr);
	}

	template <class As, class T>
	inline void processImpl(As& as, virtual_base_class<T>& b) {
		detail::base_class_id id(b.base_ptr);
		if (itsBaseClassSet.emplace(id).second)
			self().processImpl(as, *b.base_ptr);
	}

	template <class As, class T>
	inline void processImpl(As& as, DeferredData<T> const& d) {
		(void) as;
		std::function<void(void)> deferment([this, d]() { process_self(d.value); });
		itsDeferments.emplace_back(std::move(deferment));
	}

	//! Empty class specialization
	template <class As, class T>
			requires (!access::is_input_serializable<As, T> && (Flags & AllowEmptyClassElision) != 0 && std::is_empty_v<T>)
	inline void processImpl(As& as, const T& var) {
		(void) as;
		(void) var;
	}

	//! Generic serialization case
	template <class As, class T>
			requires access::is_input_serializable<As, T>
	inline void processImpl(As& as, T& var) {
		if constexpr (access::has_member_serialize<As, T>) {
			loadClassVersionUnused<T>();
			access::member_serialize(as, var);

		} else if constexpr (access::has_global_serialize<As, T>) {
			loadClassVersionUnused<T>();
			VIDE_FUNCTION_NAME_SERIALIZE(as, var);

		} else if constexpr (access::has_member_load<As, T>) {
			loadClassVersionUnused<T>();
			access::member_load(as, var);

		} else if constexpr (access::has_global_load<As, T>) {
			loadClassVersionUnused<T>();
			VIDE_FUNCTION_NAME_LOAD(as, var);

		} else if constexpr (access::has_member_load_minimal<As, T>) {
			loadClassVersionUnused<T>();
			access::get_member_save_minimal_t<As, T> value;
			self().process_as(as, value);
			access::member_load_minimal(as, var, std::move(value));

		} else if constexpr (access::has_global_load_minimal<As, T>) {
			loadClassVersionUnused<T>();
			access::get_global_save_minimal_t<As, T> value;
			self().process_as(as, value);
			VIDE_FUNCTION_NAME_LOAD_MINIMAL(as, var, std::move(value));

		} else if constexpr (access::has_member_serialize_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			access::member_serialize(as, var, version);

		} else if constexpr (access::has_global_serialize_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			VIDE_FUNCTION_NAME_SERIALIZE(as, var, version);

		} else if constexpr (access::has_member_load_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			access::member_load(as, var, version);

		} else if constexpr (access::has_global_load_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			VIDE_FUNCTION_NAME_LOAD(as, var, version);

		} else if constexpr (access::has_member_load_minimal_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			access::get_member_save_minimal_versioned_t<As, T> value;
			self().process_as(as, value);
			access::member_load_minimal(as, var, std::move(value), version);

		} else if constexpr (access::has_global_load_minimal_versioned<As, T>) {
			const auto version = loadClassVersion<T>();
			access::get_global_save_minimal_versioned_t<As, T> value;
			self().process_as(as, value);
			VIDE_FUNCTION_NAME_LOAD_MINIMAL(as, var, std::move(value), version);
		}
	}

	//! No matching serialization
	/*! Invalid if we have invalid input versioning or
		we are not input serializable, and either
		don't allow empty class ellision or allow it but are not serializing an empty class */
	template <class As, class T>
		requires (!access::is_input_serializable<As, T>)
	inline unserializable_type_tag processImpl(As& as, const T&) {
		(void) as;

		static_assert(access::count_input_serializers<ArchiveType, T> != 0, "\n"
				"Vide could not find any input serialization functions for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Serialize functions generally have the following signature:\n\n"
				"  template <class Archive>\n"
				"  void serialize(Archive& ar) {\n"
				"    ar(member1);\n"
				"    ar(member2);\n"
				"  }\n\n");

		static_assert(access::count_input_serializers<ArchiveType, T> <= 1, "\n"
				"Vide found more than one compatible input serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Note that serialization functions can be inherited which may lead to the aforementioned ambiguities.\n"
				"In addition, you may not mix versioned with non-versioned serialization functions.\n\n");

		return unserializable_type_tag{};
	}
}; // class InputArchive

} // namespace vide

// This include needs to come after things such as binary_data, make_nvp, etc
#include <vide/types/common.hpp>
