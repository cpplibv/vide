#pragma once

#include <vide/access_fwd.hpp>
#include <vide/concept.hpp>
#include <vide/macros.hpp>

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>


namespace vide { // --------------------------------------------------------------------------------

//! A class that can be made a friend to give vide access to non public functions
/*! If you desire non-public serialization functions within a class, vide can only
	access these if you declare vide::access a friend.

	@code{.cpp}
	class MyClass {
	private:
		friend class vide::access; // gives access to the private serialize

		template <class Archive>
		void serialize(Archive& ar) {
			// some code
		}
	};
	@endcode
	@ingroup Access */
struct access {
	template <class T>
	static constexpr bool has_static_member_class_version = requires {
		{ T::serialize_class_version } -> std::convertible_to<std::uint32_t>;
	};

	template <class T>
	static constexpr std::uint32_t static_member_class_version = T::serialize_class_version;

	template <typename Archive, typename T>
	using get_member_save_minimal_t = std::remove_cvref_t<decltype(
			std::declval<const T&>().VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>()))>;

	template <typename Archive, typename T>
	using get_member_save_minimal_versioned_t = std::remove_cvref_t<decltype(
			std::declval<const T&>().VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>(), std::uint32_t{0}))>;

	// --- Member shapes, see serializer<T> ------------------------------------------------------------

	template <class Archive, class T>
	static constexpr bool has_member_serialize = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_serialize_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar, std::uint32_t{0}) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_versioned = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar, std::uint32_t{0}) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar, std::uint32_t{0}) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_minimal = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar) } -> NotVoid;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_minimal_versioned = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, std::uint32_t{0}) } -> NotVoid;
	};

	// The loaded type is deduced from save_minimal, called with the input archive (unevaluated)
	template <class Archive, class T>
	static constexpr bool has_member_load_minimal = requires (Archive& ar, T& var, get_member_save_minimal_t<Archive, T> value) {
		{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(value)) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load_minimal_versioned = requires (Archive& ar, T& var, get_member_save_minimal_versioned_t<Archive, T> value) {
		{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(value), std::uint32_t{0}) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_serialize_minimal = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE_MINIMAL(ar) } -> LValueReference;
	};

	template <class Archive, class T>
	static constexpr bool has_member_serialize_minimal_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE_MINIMAL(ar, std::uint32_t{0}) } -> LValueReference;
	};

	/// Number of member shapes T provides for the direction of Archive, 1 when well-formed
	template <class Archive, class T>
	static constexpr int count_member_shapes = Archive::is_output ?
			has_member_serialize<Archive, T> + has_member_serialize_versioned<Archive, T> +
			has_member_save<Archive, T> + has_member_save_versioned<Archive, T> +
			has_member_save_minimal<Archive, T> + has_member_save_minimal_versioned<Archive, T> +
			has_member_serialize_minimal<Archive, T> + has_member_serialize_minimal_versioned<Archive, T> :
			has_member_serialize<Archive, T> + has_member_serialize_versioned<Archive, T> +
			has_member_load<Archive, T> + has_member_load_versioned<Archive, T> +
			has_member_load_minimal<Archive, T> + has_member_load_minimal_versioned<Archive, T> +
			has_member_serialize_minimal<Archive, T> + has_member_serialize_minimal_versioned<Archive, T>;

	template <class Archive, class T>
	static constexpr bool has_member_minimal = Archive::is_output ?
			has_member_save_minimal<Archive, T> || has_member_save_minimal_versioned<Archive, T> ||
			has_member_serialize_minimal<Archive, T> || has_member_serialize_minimal_versioned<Archive, T> :
			has_member_load_minimal<Archive, T> || has_member_load_minimal_versioned<Archive, T> ||
			has_member_serialize_minimal<Archive, T> || has_member_serialize_minimal_versioned<Archive, T>;

	/// Calls the member shape of T: serialize, save/load, save_minimal/load_minimal or
	/// serialize_minimal, each with or without the version parameter
	template <class Archive, class T>
	static void serialize_members(Archive& ar, T& var, std::uint32_t version) {
		static_assert(count_member_shapes<Archive, T> == 1, "\n"
				"Vide found more than one member serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, load_minimal/save_minimal pair or serialize_minimal\n"
				"(you may not mix these, nor versioned and non-versioned forms). Inherited serialization functions count too.\n"
				"Specialize vide::serializer<T> to pick one explicitly.\n\n");

		if constexpr (has_member_serialize<Archive, T>) {
			var.VIDE_FUNCTION_NAME_SERIALIZE(ar);
		} else if constexpr (has_member_serialize_versioned<Archive, T>) {
			var.VIDE_FUNCTION_NAME_SERIALIZE(ar, version);
		} else if constexpr (has_member_serialize_minimal<Archive, T>) {
			ar(var.VIDE_FUNCTION_NAME_SERIALIZE_MINIMAL(ar));
		} else if constexpr (has_member_serialize_minimal_versioned<Archive, T>) {
			ar(var.VIDE_FUNCTION_NAME_SERIALIZE_MINIMAL(ar, version));
		} else if constexpr (Archive::is_output) {
			if constexpr (has_member_save<Archive, T>)
				std::as_const(var).VIDE_FUNCTION_NAME_SAVE(ar);
			else if constexpr (has_member_save_versioned<Archive, T>)
				std::as_const(var).VIDE_FUNCTION_NAME_SAVE(ar, version);
			else if constexpr (has_member_save_minimal<Archive, T>)
				ar(std::as_const(var).VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar));
			else
				ar(std::as_const(var).VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, version));
		} else {
			if constexpr (has_member_load<Archive, T>) {
				var.VIDE_FUNCTION_NAME_LOAD(ar);
			} else if constexpr (has_member_load_versioned<Archive, T>) {
				var.VIDE_FUNCTION_NAME_LOAD(ar, version);
			} else if constexpr (has_member_load_minimal<Archive, T>) {
				get_member_save_minimal_t<Archive, T> value;
				ar(value);
				var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(value));
			} else {
				get_member_save_minimal_versioned_t<Archive, T> value;
				ar(value);
				var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(value), version);
			}
		}
	}

	// Used for placement new
	template <class T, class... Args>
	static constexpr inline void construct(T*& ptr, Args&&... args) {
		new(ptr) T(std::forward<Args>(args)...);
	}

	// Used for non-placement new with a default constructor
	template <class T>
			requires requires { T(); }
	[[nodiscard]] static constexpr inline T* construct() {
		return new T();
	}
};

/// Base for a serializer<T> specialization that keeps the class version of T: exposes
/// T::serialize_class_version, private ones included, and is empty when T declares none
template <typename T>
struct copy_class_version_of {};

template <typename T>
	requires access::has_static_member_class_version<T>
struct copy_class_version_of<T> {
	static constexpr std::uint32_t serialize_class_version = access::static_member_class_version<T>;
};

} // namespace vide --------------------------------------------------------------------------------
