#pragma once

#include <vide/details/util.hpp>
#include <vide/exception.hpp>
#include <vide/macros.hpp>
#include <vide/serializer.hpp>

#include <concepts>
#include <cstdint>
#include <meta>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>


namespace vide { // --------------------------------------------------------------------------------

namespace detail {

template <class T>
consteval std::string_view opaque_enum_message() {
	return std::define_static_string("\n"
			"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"\n"
			"    ERROR: Vide cannot serialize the enum [" VIDE_DIAGNOSTIC_HIGHLIGHT_BEGIN + std::string(std::meta::display_string_of(^^T)) + VIDE_DIAGNOSTIC_HIGHLIGHT_END "] from an opaque declaration.\n"
			"    Include its definition (with the enumerator list) before serializing it.\n"
			"\n"
			"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

} // namespace detail

struct enum_value_set {
	template <typename T>
	static constexpr bool has_unbounded_enumerator = requires {
		T::serialize_unbounded;
	};

	template <typename T>
	static constexpr bool has_end_value_enumerator = requires {
		T::serialize_end_value;
	};

	template <typename T>
	static constexpr bool has_max_value_enumerator = requires {
		T::serialize_max_value;
	};

	template <typename T>
	static constexpr bool has_unbounded_free_function = requires {
		{ serialize_enum_unbounded(std::declval<T>()) } -> std::same_as<void>;
	};

	template <typename T>
	static constexpr bool has_end_value_free_function_enum = requires {
		{ serialize_enum_end_value(std::declval<T>()) } -> std::same_as<T>;
	};

	template <typename T>
	static constexpr bool has_end_value_free_function_underlying = requires {
		{ serialize_enum_end_value(std::declval<T>()) } -> std::same_as<std::underlying_type_t<T>>;
	};

	template <typename T>
	static constexpr bool has_max_value_free_function_enum = requires {
		{ serialize_enum_max_value(std::declval<T>()) } -> std::same_as<T>;
	};

	template <typename T>
	static constexpr bool has_max_value_free_function_underlying = requires {
		{ serialize_enum_max_value(std::declval<T>()) } -> std::same_as<std::underlying_type_t<T>>;
	};

	template <typename T>
	static constexpr bool has_verify_free_function = requires {
		{ serialize_enum_verify(std::declval<T>()) } -> std::convertible_to<bool>;
	};

	template <typename T>
	static constexpr bool is_unbounded = has_unbounded_enumerator<T> || has_unbounded_free_function<T>;

	template <typename T>
	static constexpr int count_specifiers =
			has_unbounded_enumerator<T> +
			has_unbounded_free_function<T> +
			has_end_value_enumerator<T> +
			has_end_value_free_function_enum<T> +
			has_end_value_free_function_underlying<T> +
			has_max_value_enumerator<T> +
			has_max_value_free_function_enum<T> +
			has_max_value_free_function_underlying<T> +
			has_verify_free_function<T>;

	template <typename T>
	static constexpr bool is_binary_serializable =
			// Opaque enums take the checked path: verify() rejects them
			std::meta::is_enumerable_type(^^T) && (
			(VIDE_STRICT_ENUM_VALUE_SET == 0 && count_specifiers<T> == 0) || // If we are not in strict mode and it has no specifier
			(count_specifiers<T> == 1 && is_unbounded<T>)); // Or if it is properly specified as unbounded

	template <typename T>
	static constexpr void verify(const T& var) {
		static constexpr int specifiers_count = count_specifiers<T>;
		// An opaque `enum class E;` is already complete (fixed underlying type), only its enumerators are
		// missing; is_complete_type cannot tell it apart, and the enumerator based specifiers read as absent
		if constexpr (!std::meta::is_enumerable_type(^^T)) {
			static_assert(false, detail::opaque_enum_message<T>());
		} else if constexpr (specifiers_count > 0 || (VIDE_STRICT_ENUM_VALUE_SET)) {
			static_assert(specifiers_count != 0,
					"Vide could not find any 'enum value set' specification for enum type T. The 'enum value set' must be specified with one and only one of the following:\n"
					"	- Defining an enumerator `serialize_unbounded` inside the EnumType with any value. Valid values will be every underlying representation.\n"
					"	- Defining an enumerator `serialize_end_value` inside the EnumType with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining an enumerator `serialize_max_value` inside the EnumType with the max value. Valid values will be: [0..end_value].\n"
					"	- Defining a free function `serialize_enum_unbounded(EnumType)` reachable by ADL returning `void`. Valid values will be every underlying representation.\n"
					"	- Defining a free function `serialize_enum_end_value(EnumType)` reachable by ADL returning `EnumType` with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining a free function `serialize_enum_end_value(EnumType)` reachable by ADL returning `Underlying` with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining a free function `serialize_enum_max_value(EnumType)` reachable by ADL returning `EnumType` with the max value. Valid values will be: [0..max_value].\n"
					"	- Defining a free function `serialize_enum_max_value(EnumType)` reachable by ADL returning `Underlying` with the max value. Valid values will be: [0..max_value].\n"
					"	- Defining a free function `serialize_enum_verify(EnumType)` reachable by ADL returning `bool` that determines if the value is valid. Valid values will those which return true.\n");

			static_assert(specifiers_count == 1,
					"Enum type T valid 'enum value set' is not correctly specified. The 'enum value set' must be specified with one and only one of the following:\n"
					"	- Defining an enumerator `serialize_unbounded` inside the EnumType with any value. Valid values will be every underlying representation.\n"
					"	- Defining an enumerator `serialize_end_value` inside the EnumType with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining an enumerator `serialize_max_value` inside the EnumType with the max value. Valid values will be: [0..end_value].\n"
					"	- Defining a free function `serialize_enum_unbounded(EnumType)` reachable by ADL returning `void`. Valid values will be every underlying representation.\n"
					"	- Defining a free function `serialize_enum_end_value(EnumType)` reachable by ADL returning `EnumType` with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining a free function `serialize_enum_end_value(EnumType)` reachable by ADL returning `Underlying` with the max value. Valid values will be: [0..end_value).\n"
					"	- Defining a free function `serialize_enum_max_value(EnumType)` reachable by ADL returning `EnumType` with the max value. Valid values will be: [0..max_value].\n"
					"	- Defining a free function `serialize_enum_max_value(EnumType)` reachable by ADL returning `Underlying` with the max value. Valid values will be: [0..max_value].\n"
					"	- Defining a free function `serialize_enum_verify(EnumType)` reachable by ADL returning `bool` that determines if the value is valid. Valid values will those which return true.\n");

			using Underlying = std::underlying_type_t<T>;
			const auto underlying = std::to_underlying(var);
			bool fail = false;
			if constexpr (is_unbounded<T>) {
				// No check
			} else if constexpr (has_end_value_enumerator<T>) {
				fail = underlying < Underlying{0} || underlying >= std::to_underlying(T::serialize_end_value);
			} else if constexpr (has_end_value_free_function_enum<T>) {
				fail = underlying < Underlying{0} || underlying >= std::to_underlying(serialize_enum_end_value(var));
			} else if constexpr (has_end_value_free_function_underlying<T>) {
				fail = underlying < Underlying{0} || underlying >= serialize_enum_end_value(var);
			} else if constexpr (has_max_value_enumerator<T>) {
				fail = underlying < Underlying{0} || underlying > std::to_underlying(T::serialize_max_value);
			} else if constexpr (has_max_value_free_function_enum<T>) {
				fail = underlying < Underlying{0} || underlying > std::to_underlying(serialize_enum_max_value(var));
			} else if constexpr (has_max_value_free_function_underlying<T>) {
				fail = underlying < Underlying{0} || underlying > serialize_enum_max_value(var);
			} else if constexpr (has_verify_free_function<T>) {
				fail = !serialize_enum_verify(var);
			}

			if (fail)
				throw Exception("Invalid '" + util::demangledName<T>() + "' enum value '" + std::to_string(std::to_underlying(var)) + "'. The specified enum value set validation failed.");
		}
	}
};

// -------------------------------------------------------------------------------------------------

template <typename T>
	requires std::is_enum_v<T>
struct serializer<T> {
	static constexpr bool minimal = true;

	template <typename Archive>
	static void serialize(Archive& ar, T& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			enum_value_set::verify(var);
			ar(std::to_underlying(var));
		} else {
			std::underlying_type_t<T> value;
			ar(value);
			var = T{value};
			enum_value_set::verify(var);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------


// --- Default unbounded enums ---------------------------------------------------------------------

namespace std {
inline void serialize_enum_unbounded(std::byte) {}
} // namespace std
