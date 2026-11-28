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

#include <vide/access.hpp>
#include <vide/base_class.hpp>
#include <vide/binary_data.hpp>
#include <vide/binary_serializable_type.hpp>
#include <vide/concept.hpp>
#include <vide/deferred_data.hpp>
#include <vide/details/archive_base.hpp>
#include <vide/details/diagnostic.hpp>
#include <vide/details/polymorphic_helper.hpp>
#include <vide/details/polymorphic_impl_fwd.hpp>
#include <vide/details/stored_smart_ptr.hpp>
#include <vide/details/vide_types.hpp>
#include <vide/exception.hpp>
#include <vide/macros.hpp>
#include <vide/serializer.hpp>
#include <vide/size_tag.hpp>
#include <vide/types/common.hpp>
#include <vide/types/enum.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <meta>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace vide {

/// Archive traits, passed as the Flags of OutputArchive and InputArchive: is_text_archive, is_binary_archive
enum Flags {
	TextArchive = 1u << 0u,
	BinaryArchive = 1u << 1u,
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

namespace detail {

template <class T>
consteval std::string_view incomplete_type_message() {
	return diagnostic_message(
			"    ERROR: Vide cannot serialize the incomplete type [" + diagnostic_name<T>() + "].\n"
			"    Include its definition before serializing it.\n");
}

/// Names T and Archive: the instantiation context that would tell them is often dozens of lines up
template <class Archive, class T>
consteval std::string_view missing_serializer_message() {
	const auto type = std::string{std::meta::display_string_of(^^T)};
	std::string text = "    ERROR: Vide could not find a serializer for [" + diagnostic_name<T>() + "] with archive [" + diagnostic_name<Archive>() + "].\n";
	if (type.starts_with("std::") || type.starts_with("boost::"))
		text += "    Most library types are covered by the vide/types/* headers, is the matching one missing?\n";

	text += "    Specialize vide::serializer<T> OR add one of the member serializers: serialize, save/load, save_minimal/load_minimal.\n"
			"\n"
			"    As member serialize:\n"
			"\n"
			"        template <typename Archive>\n"
			"        void serialize(Archive& ar) {\n"
			"            ar.nvp(\"member1\", member1);\n"
			"        }\n"
			"\n"
			"    As global vide::serializer<T>:\n"
			"\n"
			"        template <>\n"
			"        struct vide::serializer<" + type + "> {\n"
			"            template <typename Archive>\n"
			"            static void serialize(Archive& ar, " + type + "& var, std::uint32_t) {\n"
			"                ar.nvp(\"member1\", var.member1);\n"
			"            }\n"
			"        };\n";
	return diagnostic_message(text);
}

/// True when text archives open a node for T; minimal types are written as a bare value.
/// A variable template on purpose: binary archives never evaluate minimal_with
template <bool TextArchive, class As, class T>
inline constexpr bool uses_node = false;

template <class As, class T>
inline constexpr bool uses_node<true, As, T> = !minimal_with<As, T>;

} // namespace detail

// ######################################################################
//! The base output archive class
/*! This is the base output archive for all output archives.  If you create
	a custom archive class, it should derive from this, passing itself as
	a template parameter for the ArchiveType.

	The base class provides all the functionality necessary to
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
	static constexpr bool enforce_validation = true;
	static constexpr bool is_proxy = false;
	static constexpr bool is_output = true;
	static constexpr bool is_input = false;
	/// Names values and opens a node per object: JSON, XML
	static constexpr bool is_text_archive = (Flags & vide::TextArchive) != 0;
	/// Ignores names and has binary_data(): binary, portable binary
	static constexpr bool is_binary_archive = (Flags & vide::BinaryArchive) != 0;

	// XMLOutputArchive writes the type attribute of every node
	static constexpr bool inserts_type_attribute = false;

	/// True when binary_data() can serialize a run of T in one block
	template <typename T>
	static constexpr bool supports_binary = is_binary_archive && binary_serializable_type<T>;

public:
	using size_type = vide::size_type;

public:
	//! Construct the output archive
	inline OutputArchive() = default;
	OutputArchive(const OutputArchive&) = delete;
	OutputArchive& operator=(const OutputArchive&) & = delete;
	inline OutputArchive(OutputArchive&&) noexcept = default;
	inline OutputArchive& operator=(OutputArchive&&) & noexcept = default;

public:
	//! Serializes the passed in data
	/*! This is the primary interface for serializing data with an archive */
	template <typename T, typename... Validators>
	inline ArchiveType& operator()(T&& var, const Validators&... validators) {
		auto& self = static_cast<ArchiveType&>(*this);
		if constexpr (enforce_validation)
			(validators(var), ...); // Output archive check before save
		self.process_as(self, var);
		return self;
	}

	/// Serializes a named value; binary archives ignore the name
	template <typename T, typename... Validators>
	inline ArchiveType& nvp(const char* name, T&& var, const Validators&... validators) {
		auto& self = static_cast<ArchiveType&>(*this);
		if constexpr (enforce_validation)
			(validators(var), ...); // Output archive check before save
		self.setNextName(name);
		self.process_as(self, var);
		return self;
	}

	/// Serializes the Base subobject of derivedThis
	template <typename Base, typename Derived>
	inline ArchiveType& base_class(const Derived* derivedThis) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, BaseClass<Base>(derivedThis));
		return self;
	}

	/// Serializes the virtual Base subobject of derivedThis, once per object
	template <typename Base, typename Derived>
	inline ArchiveType& virtual_base_class(const Derived* derivedThis) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, VirtualBaseClass<Base>(derivedThis));
		return self;
	}

	/// Serializes var at the next serializeDeferments() instead of here. A saved rvalue is copied until then
	template <typename T>
	inline ArchiveType& defer(T&& var) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, DeferredData<T>{std::forward<T>(var)});
		return self;
	}

	/// Serializes size elements starting at data as one block of raw memory
	template <binary_serializable_type T>
	inline ArchiveType& binary_data(const T* data, std::size_t size) requires is_binary_archive {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, BinaryData<sizeof(T)>{const_cast<T*>(data), size});
		return self;
	}

	template <typename T, typename... Validators>
	inline ArchiveType& ignore(const Validators&...) {
		// During output ignore does nothing
		return static_cast<ArchiveType&>(*this);
	}

	template <typename T, typename... Validators>
	inline ArchiveType& nvp_ignore(const char* name, const Validators&...) {
		// During output ignore does nothing
		(void) name;
		return static_cast<ArchiveType&>(*this);
	}

	/// Serializes the number of elements of a container, ahead of the elements.
	/// Throws when size does not fit size_type
	inline ArchiveType& size_tag(std::uint64_t size) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, SizeTag<size_type>{detail::checked_size<size_type>(size)});
		return self;
	}

	inline void verify(bool pass, std::string_view message) const {
		if constexpr (ArchiveType::enforce_validation)
			if (!pass)
				throw Exception("Validation failed during serialization: " + std::string(message));
	}

public:
	// Archives intercept types with more specialized process_as overloads, everything else ends up here.
	// As is the archive the serializers see: the archive itself or a proxy over it
	template <class As, class T>
	inline void process_as(As& as, const T& var) {
		// Failed checks are branches of their own: the serialization code stays uninstantiated, so a mistake
		// gives one error and no follow-ups from the call that cannot compile
		if constexpr (!std::meta::is_complete_type(^^T)) {
			static_assert(false, detail::incomplete_type_message<T>());
		} else if constexpr (!serializable_with<As, T> && !serializable_with<ArchiveType, T>) {
			static_assert(false, detail::missing_serializer_message<ArchiveType, T>());
		} else {
			auto& self = static_cast<ArchiveType&>(*this);
			static constexpr bool node = detail::uses_node<ArchiveType::is_text_archive, As, T>;
			if constexpr (node) {
				self.startNode();
				if constexpr (ArchiveType::inserts_type_attribute)
					self.template insertType<T>();
			}

			std::uint32_t version = 0;
			if constexpr (is_versioned_class<T>) {
				version = serializer<T>::serialize_class_version;
				if (registerVersionedType(typeid(T)))
					self.nvp(VIDE_CLASS_VERSION_TAG_NAME, version);
			}

			if constexpr (serializable_with<As, T>) {
				if constexpr (uses_default_serializer<T>)
					access::serialize_members(as, const_cast<T&>(var), version);
				else
					serializer<T>::serialize(as, const_cast<T&>(var), version);
			} else {
				// T cannot be serialized with the proxy type: the archive itself serializes it
				if constexpr (uses_default_serializer<T>)
					access::serialize_members(self, const_cast<T&>(var), version);
				else
					serializer<T>::serialize(self, const_cast<T&>(var), version);
			}

			if constexpr (node)
				self.finishNode();
		}
	}

	// The node of the tag is the node of the base: both are opened by the generic process_as
	template <class As, class T>
	inline void process_as(As& as, const BaseClass<T>& tag) {
		as.process_as(as, *tag.base_ptr);
	}

	// Serialized later, out of place: a name given here has no node to land on
	template <class As, class T>
	inline void process_as(As&, const DeferredData<T>& tag) {
		discardNextName();
		std::function<void()> deferment([this, tag] {
			auto& self = static_cast<ArchiveType&>(*this);
			self.process_as(self, tag.value);
		});
		addDeferment(std::move(deferment));
	}

	// Once per object: a diamond reaches the same virtual base on every path. A skipped one writes
	// nothing, not even its name
	template <class As, class T>
	inline void process_as(As& as, const VirtualBaseClass<T>& tag) {
		if (registerVirtualBaseClass(detail::base_class_id(tag.base_ptr)))
			as.process_as(as, *tag.base_ptr);
		else
			discardNextName();
	}

private:
	inline void discardNextName() {
		static_cast<ArchiveType&>(*this).setNextName(nullptr);
	}
}; // class OutputArchive

// ######################################################################
//! The base input archive class
/*! This is the base input archive for all input archives.  If you create
	a custom archive class, it should derive from this, passing itself as
	a template parameter for the ArchiveType.

	The base class provides all the functionality necessary to
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
	static constexpr bool enforce_validation = true;
	static constexpr bool is_proxy = false;
	static constexpr bool is_output = false;
	static constexpr bool is_input = true;
	/// Names values and opens a node per object: JSON, XML
	static constexpr bool is_text_archive = (Flags & vide::TextArchive) != 0;
	/// Ignores names and has binary_data(): binary, portable binary
	static constexpr bool is_binary_archive = (Flags & vide::BinaryArchive) != 0;

	/// True when binary_data() can serialize a run of T in one block
	template <typename T>
	static constexpr bool supports_binary = is_binary_archive && binary_serializable_type<T>;

public:
	using size_type = vide::size_type;

public:
	//! Construct the output archive
	inline InputArchive() = default;
	InputArchive(const InputArchive&) = delete;
	InputArchive& operator=(const InputArchive&) & = delete;
	inline InputArchive(InputArchive&&) noexcept = default;
	inline InputArchive& operator=(InputArchive&&) & noexcept = default;

public:
	//! Serializes the passed in data
	/*! This is the primary interface for serializing data with an archive */
	template <typename T, typename... Validators>
	inline ArchiveType& operator()(T&& var, const Validators&... validators) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.process_as(self, var);
		if constexpr (enforce_validation)
			(validators(var), ...); // Input archive validate after load
		return self;
	}

	/// Serializes a named value; binary archives ignore the name
	template <typename T, typename... Validators>
	inline ArchiveType& nvp(const char* name, T&& var, const Validators&... validators) {
		auto& self = static_cast<ArchiveType&>(*this);
		self.setNextName(name);
		self.process_as(self, var);
		if constexpr (enforce_validation)
			(validators(var), ...); // Input archive validate after load
		return self;
	}

	/// Serializes the Base subobject of derivedThis
	template <typename Base, typename Derived>
	inline ArchiveType& base_class(const Derived* derivedThis) {
		auto& self = static_cast<ArchiveType&>(*this);
		BaseClass<Base> tag(derivedThis);
		self.process_as(self, tag);
		return self;
	}

	/// Serializes the virtual Base subobject of derivedThis, once per object
	template <typename Base, typename Derived>
	inline ArchiveType& virtual_base_class(const Derived* derivedThis) {
		auto& self = static_cast<ArchiveType&>(*this);
		VirtualBaseClass<Base> tag(derivedThis);
		self.process_as(self, tag);
		return self;
	}

	/// Serializes var at the next serializeDeferments() instead of here
	template <typename T>
	inline ArchiveType& defer(T& var) {
		auto& self = static_cast<ArchiveType&>(*this);
		DeferredData<T&> tag{var};
		self.process_as(self, tag);
		return self;
	}

	/// Serializes size elements starting at data as one block of raw memory
	template <binary_serializable_type T>
	inline ArchiveType& binary_data(T* data, std::size_t size) requires (is_binary_archive && !std::is_const_v<T>) {
		auto& self = static_cast<ArchiveType&>(*this);
		BinaryData<sizeof(T)> tag{data, size};
		self.process_as(self, tag);
		return self;
	}

	template <typename T, typename... Validators>
	inline ArchiveType& ignore(const Validators&... validators) {
		T var;
		return (*this)(var, validators...);
	}

	template <typename T, typename... Validators>
	inline ArchiveType& nvp_ignore(const char* name, const Validators&... validators) {
		T var;
		return nvp(name, var, validators...);
	}

	template <typename T, typename... Validators>
	[[nodiscard]] inline T load(const Validators&... validators) {
		T var;
		(*this)(var, validators...);
		return var;
	}

	template <typename T, typename... Validators>
	[[nodiscard]] inline T nvp_load(const char* name, const Validators&... validators) {
		T var;
		nvp(name, var, validators...);
		return var;
	}

	/// Serializes the number of elements of a container, ahead of the elements.
	/// Untrusted: size the container through safe_to_reserve() or validate_read_size()
	inline ArchiveType& size_tag(size_type& size) {
		auto& self = static_cast<ArchiveType&>(*this);
		SizeTag<size_type&> tag{size};
		self.process_as(self, tag);
		return self;
	}

	[[nodiscard]] inline size_type size_tag() {
		size_type size{};
		size_tag(size);
		return size;
	}

	inline void verify(bool pass, std::string_view message) const {
		if constexpr (ArchiveType::enforce_validation)
			if (!pass)
				throw Exception("Validation failed during serialization: " + std::string(message));
	}

	/// Number of T to reserve up front for a container of numElements loaded elements, at most numElements.
	/// Draws on a budget of VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER times the input size, shared by the whole
	/// archive: a forged size cannot allocate more. Elements past it are added one by one, each loaded first
	template <typename T>
	[[nodiscard]] std::size_t safe_to_reserve(size_type numElements) {
		const auto requestedBytes = numElements * sizeof(T);
		const auto numSafeElements = requestedBytes <= reserveMemoryBudget ? numElements : reserveMemoryBudget / sizeof(T);
		reserveMemoryBudget -= numSafeElements * sizeof(T);
		return numSafeElements;
	}

	/// Throws unless numElements of T fit the unread input, before resizing for a binary_data() load.
	/// maximumBinaryReadSize() of the archive reports the unread bytes
	template <typename T> requires is_binary_archive
	inline void validate_read_size(size_type numElements) {
		const auto requestedBytes = numElements * sizeof(T);
		if (requestedBytes > static_cast<ArchiveType&>(*this).maximumBinaryReadSize())
			throw Exception("Read size validation of " + std::to_string(requestedBytes) + " bytes failed.");
	}

	//! Registers a class version with the archive and serializes it if necessary
	/*! If this is the first time this class has been serialized, we will record its
		version number and serialize that.

		@tparam T The type of the class being serialized */
	template <class T>
	inline std::uint32_t loadClassVersion() {
		if (const auto loaded = findLoadedVersion(typeid(T)))
			return *loaded;

		std::uint32_t version;
		static_cast<ArchiveType&>(*this).nvp(VIDE_CLASS_VERSION_TAG_NAME, version);
		addLoadedVersion(typeid(T), version);
		return version;
	}

public:
	// Archives intercept types with more specialized process_as overloads, everything else ends up here.
	// As is the archive the serializers see: the archive itself or a proxy over it
	template <class As, class T>
	inline void process_as(As& as, T& var) {
		// Failed checks are branches of their own: the serialization code stays uninstantiated, so a mistake
		// gives one error and no follow-ups from the call that cannot compile
		if constexpr (!std::meta::is_complete_type(^^T)) {
			static_assert(false, detail::incomplete_type_message<T>());
		} else if constexpr (!serializable_with<As, T> && !serializable_with<ArchiveType, T>) {
			static_assert(false, detail::missing_serializer_message<ArchiveType, T>());
		} else {
			auto& self = static_cast<ArchiveType&>(*this);
			static constexpr bool node = detail::uses_node<ArchiveType::is_text_archive, As, T>;
			if constexpr (node)
				self.startNode();

			std::uint32_t version = 0;
			if constexpr (is_versioned_class<T>)
				version = loadClassVersion<T>();

			if constexpr (serializable_with<As, T>) {
				if constexpr (uses_default_serializer<T>)
					access::serialize_members(as, var, version);
				else
					serializer<T>::serialize(as, var, version);
			} else {
				// T cannot be serialized with the proxy type: the archive itself serializes it
				if constexpr (uses_default_serializer<T>)
					access::serialize_members(self, var, version);
				else
					serializer<T>::serialize(self, var, version);
			}

			if constexpr (node)
				self.finishNode();
		}
	}

	// The node of the tag is the node of the base: both are opened by the generic process_as
	template <class As, class T>
	inline void process_as(As& as, BaseClass<T>& tag) {
		as.process_as(as, *tag.base_ptr);
	}

	// Serialized later, out of place: a name given here has no node to land on
	template <class As, class T>
	inline void process_as(As&, DeferredData<T>& tag) {
		discardNextName();
		std::function<void()> deferment([this, tag] {
			auto& self = static_cast<ArchiveType&>(*this);
			self.process_as(self, tag.value);
		});
		addDeferment(std::move(deferment));
	}

	// Once per object: a diamond reaches the same virtual base on every path. A skipped one writes
	// nothing, not even its name
	template <class As, class T>
	inline void process_as(As& as, VirtualBaseClass<T>& tag) {
		if (registerVirtualBaseClass(detail::base_class_id(tag.base_ptr)))
			as.process_as(as, *tag.base_ptr);
		else
			discardNextName();
	}

private:
	inline void discardNextName() {
		static_cast<ArchiveType&>(*this).setNextName(nullptr);
	}
}; // class InputArchive

} // namespace vide
