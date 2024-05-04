#pragma once

#include <type_traits>
#include <utility>

#include <vide/access_fwd.hpp>
#include <vide/concept.hpp>
#include <vide/macros.hpp>


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

	// --- Traits --------------------------------------------------------------------------------------

	template <typename Archive, typename T>
	using get_member_save_minimal_t = std::remove_cvref_t<decltype(
			std::declval<const T&>().VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>()))>;

	template <typename Archive, typename T>
	using get_global_save_minimal_t = std::remove_cvref_t<decltype(
			VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>(), std::declval<const T&>()))>;

	template <typename Archive, typename T>
	using get_member_save_minimal_versioned_t = std::remove_cvref_t<decltype(
			std::declval<const T&>().VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>(), 0))>;

	template <typename Archive, typename T>
	using get_global_save_minimal_versioned_t = std::remove_cvref_t<decltype(
			VIDE_FUNCTION_NAME_SAVE_MINIMAL(std::declval<Archive&>(), std::declval<const T&>(), 0))>;

	// Standard

	template <class Archive, class T>
	static constexpr bool has_member_serialize = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_serialize = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_SERIALIZE(ar, var) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_save = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE(ar, var) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_load = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_LOAD(ar, var) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_minimal = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar) } -> NotVoid;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load_minimal = requires (Archive& ar, T& var, get_member_save_minimal_t<Archive, T> mvalue) {
		{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(mvalue)) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_save_minimal = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, var) } -> NotVoid;
	};

	template <class Archive, class T>
	static constexpr bool has_global_load_minimal = requires (Archive& ar, T& var, get_global_save_minimal_t<Archive, T> mvalue) {
		{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, std::move(mvalue)) } -> Void;
	};

	// Versioned

	template <class Archive, class T>
	static constexpr bool has_member_serialize_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_serialize_versioned = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_SERIALIZE(ar, var, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_versioned = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_save_versioned = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE(ar, var, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_load_versioned = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_LOAD(ar, var, 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_member_save_minimal_versioned = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, 0) } -> NotVoid;
	};

	template <class Archive, class T>
	static constexpr bool has_member_load_minimal_versioned = requires (Archive& ar, T& var, get_member_save_minimal_versioned_t<Archive, T> mvalue) {
		{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::move(mvalue), 0) } -> Void;
	};

	template <class Archive, class T>
	static constexpr bool has_global_save_minimal_versioned = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, var, 0) } -> NotVoid;
	};

	template <class Archive, class T>
	static constexpr bool has_global_load_minimal_versioned = requires (Archive& ar, T& var, get_global_save_minimal_versioned_t<Archive, T> mvalue) {
		{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, std::move(mvalue), 0) } -> Void;
	};

	// --- Error Detection -----------------------------------------------------------------------------

	template <class Archive, class T>
	static constexpr bool error_has_member_save_non_const = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar) } -> Void;
	};
	template <class Archive, class T>
	static constexpr bool error_has_member_save_minimal_non_const = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar) } -> NotVoid;
	};
	template <class Archive, class T>
	static constexpr bool error_has_member_save_versioned_non_const = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar, std::uint32_t{0}) } -> Void;
	};
	template <class Archive, class T>
	static constexpr bool error_has_member_save_minimal_versioned_non_const = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar, std::uint32_t{0}) } -> NotVoid;
	};

	// --- Conclusions ---------------------------------------------------------------------------------

	template <class Archive, class T>
	static constexpr int has_minimal_input_serialization =
			has_member_load_minimal<Archive, T> +
			has_global_load_minimal<Archive, T> +
			has_member_load_minimal_versioned<Archive, T> +
			has_global_load_minimal_versioned<Archive, T>;

	template <class Archive, class T>
	static constexpr int has_minimal_output_serialization =
			has_member_save_minimal<Archive, T> +
			has_global_save_minimal<Archive, T> +
			has_member_save_minimal_versioned<Archive, T> +
			has_global_save_minimal_versioned<Archive, T>;

	template <class Archive, class T>
	static constexpr int count_output_serializers =
			has_member_serialize<Archive, T> +
			has_global_serialize<Archive, T> +
			has_member_save<Archive, T> +
			has_global_save<Archive, T> +
			has_member_save_minimal<Archive, T> +
			has_global_save_minimal<Archive, T> +
			has_member_serialize_versioned<Archive, T> +
			has_global_serialize_versioned<Archive, T> +
			has_member_save_versioned<Archive, T> +
			has_global_save_versioned<Archive, T> +
			has_member_save_minimal_versioned<Archive, T> +
			has_global_save_minimal_versioned<Archive, T>;

	template <class Archive, class T>
	static constexpr int count_input_serializers =
			has_member_serialize<Archive, T> +
			has_global_serialize<Archive, T> +
			has_member_load<Archive, T> +
			has_global_load<Archive, T> +
			has_member_load_minimal<Archive, T> +
			has_global_load_minimal<Archive, T> +
			has_member_serialize_versioned<Archive, T> +
			has_global_serialize_versioned<Archive, T> +
			has_member_load_versioned<Archive, T> +
			has_global_load_versioned<Archive, T> +
			has_member_load_minimal_versioned<Archive, T> +
			has_global_load_minimal_versioned<Archive, T>;

	template <class Archive, class T>
	static constexpr bool is_input_serializable = count_input_serializers<Archive, T> == 1;
	template <class Archive, class T>
	static constexpr bool is_output_serializable = count_output_serializers<Archive, T> == 1;

	// --- Standard serialzation access ----------------------------------------------------------------

	template <class Archive, class T>
	inline static decltype(auto) member_serialize(Archive& ar, T& var) {
		return var.VIDE_FUNCTION_NAME_SERIALIZE(ar);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_save(Archive& ar, const T& var) {
		return var.VIDE_FUNCTION_NAME_SAVE(ar);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_load(Archive& ar, T& var) {
		return var.VIDE_FUNCTION_NAME_LOAD(ar);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_save_minimal(Archive& ar, const T& var) {
		return var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar);
	}

	template <class Archive, class T, class U>
	inline static decltype(auto) member_load_minimal(Archive& ar, T& var, U&& value) {
		return var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::forward<U>(value));
	}

	// --- Versioned serialzation access ---------------------------------------------------------------

	template <class Archive, class T>
	inline static decltype(auto) member_serialize(Archive& ar, T& var, const std::uint32_t version) {
		return var.VIDE_FUNCTION_NAME_SERIALIZE(ar, version);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_save(Archive& ar, const T& var, const std::uint32_t version) {
		return var.VIDE_FUNCTION_NAME_SAVE(ar, version);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_load(Archive& ar, T& var, const std::uint32_t version) {
		return var.VIDE_FUNCTION_NAME_LOAD(ar, version);
	}

	template <class Archive, class T>
	inline static decltype(auto) member_save_minimal(Archive& ar, const T& var, const std::uint32_t version) {
		return var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, version);
	}

	template <class Archive, class T, class U>
	inline static decltype(auto) member_load_minimal(Archive& ar, T& var, U&& value, const std::uint32_t version) {
		return var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, std::forward<U>(value), version);
	}

	// --- Other Functionality -------------------------------------------------------------------------

	// Used for detecting inheritance from enable_shared_from_this
	template <class T>
	inline static auto shared_from_this(T& t) -> decltype(t.shared_from_this());

	// Used for placement new
	template <class T, class... Args>
	inline static void construct(T*& ptr, Args&&... args) {
		new(ptr) T(std::forward<Args>(args)...);
	}

	// Used for non-placement new with a default constructor
	template <class T>
			requires requires { T(); }
	inline static T* construct() {
		return new T();
	}
};

} // namespace vide --------------------------------------------------------------------------------
