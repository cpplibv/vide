/*! \file cereal.hpp
    \brief Main cereal functionality */
/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_CEREAL_HPP_
#define CEREAL_CEREAL_HPP_

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cereal/details/bits.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/binary_data.hpp>
#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>
#include <cereal/size_tag.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/unserializable_type_tag.hpp>


namespace cereal {

// ######################################################################
//! Marks data for deferred serialization
/*! cereal performs a recursive depth-first traversal of data it serializes. When
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
		archive( cereal::defer( connection ),
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
	return {std::forward<T>(value)};
}

// ######################################################################
//! Called before a type is serialized to set up any special archive state
//! for processing some type
/*! If designing a serializer that needs to set up any kind of special
	state or output extra information for a type, specialize this function
	for the archive type and the types that require the extra information.
	@ingroup Internal */
template <class Archive, class T>
inline void prologue(Archive& /* archive */, const T& /* data */) {}

//! Called after a type is serialized to tear down any special archive state
//! for processing some type
/*! @ingroup Internal */
template <class Archive, class T>
inline void epilogue(Archive& /* archive */, const T& /* data */) {}

// ######################################################################
//! Special flags for archives
/*! AllowEmptyClassElision
	  This allows for empty classes to be serialized even if they do not provide
	  a serialization function.  Classes with no data members are considered to be
	  empty.  Be warned that if this is enabled and you attempt to serialize an
	  empty class with improperly formed serialize or load/save functions, no
	  static error will occur - the error will propogate silently and your
	  intended serialization functions may not be called.  You can manually
	  ensure that your classes that have custom serialization are correct
	  by using the traits is_output_serializable and is_input_serializable
	  in cereal/details/traits.hpp.
	@ingroup Internal */
enum Flags {
	AllowEmptyClassElision = 1u << 0u,
	IgnoreNVP = 1u << 1u,
	//	____ = 1u << 2u,
	//	____ = 1u << 3u,
	//	____ = 1u << 4u,
	//	____ = 1u << 5u,
};

// ######################################################################
//! Registers a specific Archive type with cereal
/*! This registration should be done once per archive.  A good place to
	put this is immediately following the definition of your archive.
	Archive registration is only strictly necessary if you wish to
	support pointers to polymorphic data types.  All archives that
	come with cereal are already registered.
	@ingroup Internal */
#define CEREAL_REGISTER_ARCHIVE(Archive)                                \
  namespace cereal { namespace detail {                                 \
  template <class T, class BindingTag>                                  \
  typename polymorphic_serialization_support<Archive, T>::type          \
  instantiate_polymorphic_binding( T*, Archive*, BindingTag, adl_tag ); \
  } } /* end namespaces */

//! Helper macro to omit unused warning
#if defined(__GNUC__)
// GCC / clang don't want the function
	#define CEREAL_UNUSED_FUNCTION
#else
	#define CEREAL_UNUSED_FUNCTION static void unused() { (void)version; }
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
	some type.  cereal will then use this value as a second parameter
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
	CEREAL_CLASS_VERSION( Mytype, 77 ); // register class version

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
#define CEREAL_CLASS_VERSION(TYPE, VERSION_NUMBER)                               \
  namespace cereal { namespace detail {                                          \
    template <> struct Version<TYPE>                                             \
    {                                                                            \
      static std::uint32_t registerVersion()                                     \
      {                                                                          \
        ::cereal::detail::StaticObject<Versions>::getInstance().mapping.emplace( \
             std::type_index(typeid(TYPE)).hash_code(), VERSION_NUMBER );        \
        return VERSION_NUMBER;                                                   \
      }                                                                          \
      static inline const std::uint32_t version = registerVersion();             \
      CEREAL_UNUSED_FUNCTION                                                     \
    }; /* end Version */                                                         \
  } } // end namespaces

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

	@tparam ArchiveType The archive type that derives from OutputArchive
	@tparam Flags Flags to control advanced functionality.  See the Flags
				  enum for more information.
	@ingroup Internal */
template <class ArchiveType, std::uint32_t Flags = 0>
class OutputArchive : public detail::OutputArchiveBase {
public:
	static constexpr bool ignores_nvp = (Flags & cereal::IgnoreNVP) != 0;

	template <typename T>
	static constexpr bool could_serialize = not requires(ArchiveType& ar, const T& t) {
		{ ar.processImpl(t) } -> std::same_as<unserializable_type_tag>;
	};

private:
	//! A set of all base classes that have been serialized
	std::unordered_set<traits::detail::base_class_id, traits::detail::base_class_id_hash> itsBaseClassSet;

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
	std::unordered_set<size_type> itsVersionedTypes;

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
		self().process(std::forward<T>(arg));
		return self();
	}

	//! Serializes any data marked for deferment using defer
	/*! This will cause any data wrapped in DeferredData to be immediately serialized */
	void serializeDeferments() {
		for (auto& deferment : itsDeferments)
			deferment();
	}

	/*! @name Boost Transition Layer
		Functionality that mirrors the syntax for Boost.  This is useful if you are transitioning
		a large project from Boost to cereal.  The preferred interface for cereal is using operator(). */
	//! @{

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		cereal.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator&(T&& arg) {
		self().process(std::forward<T>(arg));
		return self();
	}

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		cereal.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator<<(T&& arg) {
		self().process(std::forward<T>(arg));
		return self();
	}

	//! @}

	//! Registers a shared pointer with the archive
	/*! This function is used to track shared pointer targets to prevent
		unnecessary saves from taking place if multiple shared pointers
		point to the same data.

		@internal
		@param sharedPointer The shared pointer itself (the adress is taked via get()).
							 The archive takes a copy to prevent the memory location to be freed
							 as long as the address is used as id. This is needed to prevent CVE-2020-11105.
		@return A key that uniquely identifies the pointer */
	inline std::uint32_t registerSharedPointer(const std::shared_ptr<const void>& sharedPointer) {
		const void* addr = sharedPointer.get();

		// Handle null pointers by just returning 0
		if (addr == nullptr) return 0;
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

private:
	//! Serializes data after calling prologue, then calls epilogue
	template <class T>
	inline void process(T&& var) {
		prologue(self(), var);
		self().processImpl(var);
		epilogue(self(), var);
	}

	//! Serialization of a virtual_base_class wrapper
	/*! \sa virtual_base_class */
	template <class T>
	inline void processImpl(virtual_base_class<T> const& b) {
		traits::detail::base_class_id id(b.base_ptr);
		if (itsBaseClassSet.count(id) == 0) {
			itsBaseClassSet.insert(id);
			self().processImpl(*b.base_ptr);
		}
	}

	//! Serialization of a base_class wrapper
	/*! \sa base_class */
	template <class T>
	inline void processImpl(base_class<T> const& b) {
		self().processImpl(*b.base_ptr);
	}

	template <class T>
	inline void processImpl(DeferredData<T> const& d) {
		std::function<void(void)> deferment([this, d]() { self().process(d.value); });
		itsDeferments.emplace_back(std::move(deferment));
	}

	//! Helper macro that expands the requirements for activating an overload
	/*! Requirements:
		  Has the requested serialization function
		  Does not have version and unversioned at the same time
		  Is output serializable AND
			is specialized for this type of function OR
			has no specialization at all */
#define PROCESS_IF(name)                                                             \
      traits::EnableIf<traits::has_##name<T, ArchiveType>::value,                          \
                       !traits::has_invalid_output_versioning<T, ArchiveType>::value,      \
                       (traits::is_output_serializable<T, ArchiveType>::value &&           \
                        (traits::is_specialized_##name<T, ArchiveType>::value ||           \
                         !traits::is_specialized<T, ArchiveType>::value))> = traits::sfinae

	//! Member serialization
	template <class T, PROCESS_IF(member_serialize)>
	inline void processImpl(const T& t) {
		access::member_serialize(self(), const_cast<T&>(t));
	}

	//! Non member serialization
	template <class T, PROCESS_IF(non_member_serialize)>
	inline void processImpl(const T& t) {
		CEREAL_SERIALIZE_FUNCTION_NAME(self(), const_cast<T&>(t));
	}

	//! Member split (save)
	template <class T, PROCESS_IF(member_save)>
	inline void processImpl(const T& t) {
		access::member_save(self(), t);
	}

	//! Non member split (save)
	template <class T, PROCESS_IF(non_member_save)>
	inline void processImpl(const T& t) {
		CEREAL_SAVE_FUNCTION_NAME(self(), t);
	}

	//! Member split (save_minimal)
	template <class T, PROCESS_IF(member_save_minimal)>
	inline void processImpl(const T& t) {
		self().process(access::member_save_minimal(self(), t));
	}

	//! Non member split (save_minimal)
	template <class T, PROCESS_IF(non_member_save_minimal)>
	inline void processImpl(const T& t) {
		self().process(CEREAL_SAVE_MINIMAL_FUNCTION_NAME(self(), t));
	}

	//! Empty class specialization
	template <class T, traits::EnableIf<(Flags & AllowEmptyClassElision),
			!traits::is_output_serializable<T, ArchiveType>::value,
			std::is_empty<T>::value> = traits::sfinae>
	inline void processImpl(const T&) {
	}

	//! No matching serialization
	/*! Invalid if we have invalid output versioning or
		we are not output serializable, and either
		don't allow empty class ellision or allow it but are not serializing an empty class */
	template <class T, traits::EnableIf<traits::has_invalid_output_versioning<T, ArchiveType>::value ||
			(!traits::is_output_serializable<T, ArchiveType>::value &&
					(!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !std::is_empty<T>::value)))> = traits::sfinae>
	//	void processImpl(const T&) {
	inline unserializable_type_tag processImpl(const T&) {
		static_assert(traits::detail::count_output_serializers<T, ArchiveType>::value != 0,
				"cereal could not find any output serialization functions for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Serialize functions generally have the following signature:\n\n"
				"  template<class Archive>\n"
				"  void serialize(Archive& ar) {\n"
				"    ar(member1);\n"
				"    ar(member2);\n"
				"  }\n\n");

		static_assert(traits::detail::count_output_serializers<T, ArchiveType>::value > 1,
				"cereal found more than one compatible output serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Use specialization (see access.hpp) if you need to disambiguate between serialize vs load/save functions. \n"
				"Note that serialization functions can be inherited which may lead to the aforementioned ambiguities.\n"
				"In addition, you may not mix versioned with non-versioned serialization functions.\n\n");

		return unserializable_type_tag{};
	}

	//! Registers a class version with the archive and serializes it if necessary
	/*! If this is the first time this class has been serialized, we will record its
		version number and serialize that.

		@tparam T The type of the class being serialized */
	template <class T>
	inline std::uint32_t registerClassVersion() {
		static const auto hash = std::type_index(typeid(T)).hash_code();
		const auto insertResult = itsVersionedTypes.insert(hash);
		const auto lock = detail::StaticObject<detail::Versions>::lock();
		const auto version =
				detail::StaticObject<detail::Versions>::getInstance().find(hash, detail::Version<T>::version);

		if (insertResult.second) // insertion took place, serialize the version number
			process(make_nvp<ArchiveType>("cereal_class_version", version));

		return version;
	}

	//! Member serialization
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_serialize)>
	inline void processImpl(const T& t) {
		access::member_serialize(self(), const_cast<T&>(t), registerClassVersion<T>());
	}

	//! Non member serialization
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_serialize)>
	inline void processImpl(const T& t) {
		CEREAL_SERIALIZE_FUNCTION_NAME(self(), const_cast<T&>(t), registerClassVersion<T>());
	}

	//! Member split (save)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_save)>
	inline void processImpl(const T& t) {
		access::member_save(self(), t, registerClassVersion<T>());
	}

	//! Non member split (save)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_save)>
	inline void processImpl(const T& t) {
		CEREAL_SAVE_FUNCTION_NAME(self(), t, registerClassVersion<T>());
	}

	//! Member split (save_minimal)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_save_minimal)>
	inline void processImpl(const T& t) {
		self().process(access::member_save_minimal(self(), t, registerClassVersion<T>()));
	}

	//! Non member split (save_minimal)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_save_minimal)>
	inline void processImpl(const T& t) {
		self().process(CEREAL_SAVE_MINIMAL_FUNCTION_NAME(self(), t, registerClassVersion<T>()));
	}

#undef PROCESS_IF
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

	@tparam ArchiveType The archive type that derives from InputArchive
	@tparam Flags Flags to control advanced functionality.  See the Flags
				  enum for more information.
	@ingroup Internal */
template <class ArchiveType, std::uint32_t Flags = 0>
class InputArchive : public detail::InputArchiveBase {
public:
	static constexpr bool ignores_nvp = (Flags & cereal::IgnoreNVP) != 0;

	template <typename T>
	static constexpr bool could_serialize = not requires(ArchiveType& ar, T& t) {
		{ ar.processImpl(t) } -> std::same_as<unserializable_type_tag>;
	};

private:
	//! A set of all base classes that have been serialized
	std::unordered_set<traits::detail::base_class_id, traits::detail::base_class_id_hash> itsBaseClassSet;

	//! Maps from pointer ids to metadata
	std::unordered_map<std::uint32_t, std::shared_ptr<void>> itsSharedPointerMap;

	//! Maps from name ids to names
	std::unordered_map<std::uint32_t, std::string> itsPolymorphicTypeMap;

	//! Maps from type hash codes to version numbers
	std::unordered_map<std::size_t, std::uint32_t> itsVersionedTypes;

	//! Deferments
	std::vector<std::function<void(void)>> itsDeferments;

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
		self().process(std::forward<T>(arg));
		return self();
	}

	//! Serializes any data marked for deferment using defer
	/*! This will cause any data wrapped in DeferredData to be immediately serialized */
	void serializeDeferments() {
		for (auto& deferment : itsDeferments)
			deferment();
	}

	/*! @name Boost Transition Layer
		Functionality that mirrors the syntax for Boost.  This is useful if you are transitioning
		a large project from Boost to cereal.  The preferred interface for cereal is using operator(). */
	//! @{

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		cereal.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator&(T&& arg) {
		self().process(std::forward<T>(arg));
		return self();
	}

	//! Serializes passed in data
	/*! This is a boost compatability layer and is not the preferred way of using
		cereal.  If you are transitioning from boost, use this until you can
		transition to the operator() overload */
	template <class T>
	inline ArchiveType& operator>>(T&& arg) {
		self().process(std::forward<T>(arg));
		return self();
	}

	//! @}

	//! Retrieves a shared pointer given a unique key for it
	/*! This is used to retrieve a previously registered shared_ptr
		which has already been loaded.

		@internal
		@param id The unique id that was serialized for the pointer
		@return A shared pointer to the data
		@throw Exception if the id does not exist */
	inline std::shared_ptr<void> getSharedPointer(std::uint32_t const id) {
		if (id == 0) return std::shared_ptr<void>(nullptr);

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
	inline void registerSharedPointer(std::uint32_t const id, std::shared_ptr<void> ptr) {
		std::uint32_t const stripped_id = id & ~detail::msb_32bit;
		itsSharedPointerMap[stripped_id] = ptr;
	}

	//! Retrieves the string for a polymorphic type given a unique key for it
	/*! This is used to retrieve a string previously registered during
		a polymorphic load.

		@internal
		@param id The unique id that was serialized for the polymorphic type
		@return The string identifier for the tyep */
	inline std::string getPolymorphicName(std::uint32_t const id) {
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
		@param name The name associated with the tyep */
	inline void registerPolymorphicName(std::uint32_t const id, std::string const& name) {
		std::uint32_t const stripped_id = id & ~detail::msb_32bit;
		itsPolymorphicTypeMap.insert({stripped_id, name});
	}

private:
	//! Serializes data after calling prologue, then calls epilogue
	template <class T>
	inline void process(T&& var) {
		prologue(self(), var);
		self().processImpl(var);
		epilogue(self(), var);
	}

	//! Serialization of a virtual_base_class wrapper
	/*! \sa virtual_base_class */
	template <class T>
	inline void processImpl(virtual_base_class<T>& b) {
		traits::detail::base_class_id id(b.base_ptr);
		if (itsBaseClassSet.count(id) == 0) {
			itsBaseClassSet.insert(id);
			self().processImpl(*b.base_ptr);
		}
	}

	//! Serialization of a base_class wrapper
	/*! \sa base_class */
	template <class T>
	inline void processImpl(base_class<T>& b) {
		self().processImpl(*b.base_ptr);
	}

	template <class T>
	inline void processImpl(DeferredData<T> const& d) {
		std::function<void(void)> deferment([this, d]() { self().process(d.value); });
		itsDeferments.emplace_back(std::move(deferment));
	}

	//! Helper macro that expands the requirements for activating an overload
	/*! Requirements:
		  Has the requested serialization function
		  Does not have version and unversioned at the same time
		  Is input serializable AND
			is specialized for this type of function OR
			has no specialization at all */
#define PROCESS_IF(name)                                                              \
      traits::EnableIf<traits::has_##name<T, ArchiveType>::value,                           \
                       !traits::has_invalid_input_versioning<T, ArchiveType>::value,        \
                       (traits::is_input_serializable<T, ArchiveType>::value &&             \
                        (traits::is_specialized_##name<T, ArchiveType>::value ||            \
                         !traits::is_specialized<T, ArchiveType>::value))> = traits::sfinae

	//! Member serialization
	template <class T, PROCESS_IF(member_serialize)>
	inline void processImpl(T& t) {
		access::member_serialize(self(), t);
	}

	//! Non member serialization
	template <class T, PROCESS_IF(non_member_serialize)>
	inline void processImpl(T& t) {
		CEREAL_SERIALIZE_FUNCTION_NAME(self(), t);
	}

	//! Member split (load)
	template <class T, PROCESS_IF(member_load)>
	inline void processImpl(T& t) {
		access::member_load(self(), t);
	}

	//! Non member split (load)
	template <class T, PROCESS_IF(non_member_load)>
	inline void processImpl(T& t) {
		CEREAL_LOAD_FUNCTION_NAME(self(), t);
	}

	//! Member split (load_minimal)
	template <class T, PROCESS_IF(member_load_minimal)>
	inline void processImpl(T& t) {
		traits::get_member_save_minimal_type<ArchiveType, T> value;
		self().process(value);
		access::member_load_minimal(self(), t, value);
	}

	//! Non member split (load_minimal)
	template <class T, PROCESS_IF(non_member_load_minimal)>
	inline void processImpl(T& t) {
		traits::get_non_member_save_minimal_type<ArchiveType, T> value;
		self().process(value);
		CEREAL_LOAD_MINIMAL_FUNCTION_NAME(self(), t, value);
	}

	//! Empty class specialization
	template <class T, traits::EnableIf<(Flags & AllowEmptyClassElision),
			!traits::is_input_serializable<T, ArchiveType>::value,
			std::is_empty<T>::value> = traits::sfinae>
	inline void processImpl(const T&) {
	}

	//! No matching serialization
	/*! Invalid if we have invalid input versioning or
		we are not input serializable, and either
		don't allow empty class ellision or allow it but are not serializing an empty class */
	template <class T, traits::EnableIf<traits::has_invalid_input_versioning<T, ArchiveType>::value ||
			(!traits::is_input_serializable<T, ArchiveType>::value &&
					(!(Flags & AllowEmptyClassElision) || ((Flags & AllowEmptyClassElision) && !std::is_empty<T>::value)))> = traits::sfinae>
	inline unserializable_type_tag processImpl(const T&) {
		static_assert(traits::detail::count_input_serializers<T, ArchiveType>::value != 0,
				"cereal could not find any input serialization functions for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Serialize functions generally have the following signature:\n\n"
				"  template <class Archive>\n"
				"  void serialize(Archive& ar) {\n"
				"    ar(member1);\n"
				"    ar(member2);\n"
				"  }\n\n");

		static_assert(traits::detail::count_input_serializers<T, ArchiveType>::value > 1,
				"cereal found more than one compatible input serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, or load_minimal/save_minimal pair (you may not mix these).\n"
				"Use specialization (see access.hpp) if you need to disambiguate between serialize vs load/save functions. \n"
				"Note that serialization functions can be inherited which may lead to the aforementioned ambiguities.\n"
				"In addition, you may not mix versioned with non-versioned serialization functions.\n\n");

		return unserializable_type_tag{};
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
		else // need to load
		{
			std::uint32_t version;

			process(make_nvp<ArchiveType>("cereal_class_version", version));
			itsVersionedTypes.emplace_hint(lookupResult, hash, version);

			return version;
		}
	}

	//! Member serialization
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_serialize)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		access::member_serialize(self(), t, version);
	}

	//! Non member serialization
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_serialize)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		CEREAL_SERIALIZE_FUNCTION_NAME(self(), t, version);
	}

	//! Member split (load)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_load)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		access::member_load(self(), t, version);
	}

	//! Non member split (load)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_load)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		CEREAL_LOAD_FUNCTION_NAME(self(), t, version);
	}

	//! Member split (load_minimal)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(member_versioned_load_minimal)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		traits::get_member_versioned_save_minimal_type<ArchiveType, T> value;
		self().process(value);
		access::member_load_minimal(self(), t, value, version);
	}

	//! Non member split (load_minimal)
	/*! Versioning implementation */
	template <class T, PROCESS_IF(non_member_versioned_load_minimal)>
	inline void processImpl(T& t) {
		const auto version = loadClassVersion<T>();
		traits::get_non_member_versioned_save_minimal_type<ArchiveType, T> value;
		self().process(value);
		CEREAL_LOAD_MINIMAL_FUNCTION_NAME(self(), t, value, version);
	}

#undef PROCESS_IF
}; // class InputArchive

} // namespace cereal

// This include needs to come after things such as binary_data, make_nvp, etc
#include <cereal/types/common.hpp>


#endif // CEREAL_CEREAL_HPP_
