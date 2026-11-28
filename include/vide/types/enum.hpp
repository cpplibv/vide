#pragma once

#include <vide/details/diagnostic.hpp>
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
	return diagnostic_message(
			"    ERROR: Vide cannot serialize the enum [" + diagnostic_name<T>() + "] from an opaque declaration.\n"
			"    Include its definition (with the enumerator list) before serializing it.\n");
}

inline constexpr std::string_view enum_domain_options =
		"    Specify exactly one domain, as an enumerator or as a free function found by ADL:\n"
		"\n"
		"        serialize_unbounded                    enumerator; every underlying value is valid\n"
		"        serialize_end_value                    enumerator; valid: [0, end_value)\n"
		"        serialize_max_value                    enumerator; valid: [0, max_value]\n"
		"        void serialize_enum_unbounded(E)       every underlying value is valid\n"
		"        E serialize_enum_end_value(E)          valid: [0, end_value); may return the underlying type\n"
		"        E serialize_enum_max_value(E)          valid: [0, max_value]; may return the underlying type\n"
		"        bool serialize_enum_verify(E)          valid: the values it returns true for\n";

} // namespace detail

struct enum_domain {
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
	static consteval std::string_view missing_domain_message() {
		return detail::diagnostic_message(
				"    ERROR: Vide found no domain for the enum [" + detail::diagnostic_name<T>() + "], VIDE_STRICT_ENUM_DOMAIN requires one.\n" +
				std::string(detail::enum_domain_options));
	}

	/// Lists the domains found, "serialize_end_value, serialize_enum_verify(E)": the clash may come from an
	/// ADL function far from the enum
	template <typename T>
	static consteval std::string_view ambiguous_domain_message() {
		std::string found;
		const auto add = [&found](bool has, std::string_view name) {
			if (!has)
				return;
			if (!found.empty())
				found += ", ";
			found += name;
		};
		add(has_unbounded_enumerator<T>, "serialize_unbounded");
		add(has_end_value_enumerator<T>, "serialize_end_value");
		add(has_max_value_enumerator<T>, "serialize_max_value");
		add(has_unbounded_free_function<T>, "serialize_enum_unbounded(E)");
		add(has_end_value_free_function_enum<T> || has_end_value_free_function_underlying<T>, "serialize_enum_end_value(E)");
		add(has_max_value_free_function_enum<T> || has_max_value_free_function_underlying<T>, "serialize_enum_max_value(E)");
		add(has_verify_free_function<T>, "serialize_enum_verify(E)");

		return detail::diagnostic_message(
				"    ERROR: Vide found more than one domain for the enum [" + detail::diagnostic_name<T>() + "]: " + found + ".\n" +
				std::string(detail::enum_domain_options));
	}

	template <typename T>
	static constexpr bool is_binary_serializable =
			// Opaque enums take the checked path: verify() rejects them
			std::meta::is_enumerable_type(^^T) && (
			(VIDE_STRICT_ENUM_DOMAIN == 0 && count_specifiers<T> == 0) || // If we are not in strict mode and it has no specifier
			(count_specifiers<T> == 1 && is_unbounded<T>)); // Or if it is properly specified as unbounded

	template <typename T>
	static constexpr void verify(const T& var) {
		static constexpr int specifiers_count = count_specifiers<T>;
		// An opaque `enum class E;` is already complete (fixed underlying type), only its enumerators are
		// missing; is_complete_type cannot tell it apart, and the enumerator based specifiers read as absent
		if constexpr (!std::meta::is_enumerable_type(^^T)) {
			static_assert(false, detail::opaque_enum_message<T>());
		} else if constexpr (specifiers_count == 0 && VIDE_STRICT_ENUM_DOMAIN) {
			static_assert(false, missing_domain_message<T>());
		} else if constexpr (specifiers_count > 1) {
			static_assert(false, ambiguous_domain_message<T>());
		} else if constexpr (specifiers_count == 1) {
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
				throw Exception("Invalid '" + util::demangledName<T>() + "' enum value '" + std::to_string(std::to_underlying(var)) + "'. The value is outside the enum domain.");
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
			enum_domain::verify(var);
			ar(std::to_underlying(var));
		} else {
			std::underlying_type_t<T> value;
			ar(value);
			var = T{value};
			enum_domain::verify(var);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------


// --- Default unbounded enums ---------------------------------------------------------------------

namespace std {
inline void serialize_enum_unbounded(std::byte) {}
} // namespace std
