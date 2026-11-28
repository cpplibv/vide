//

#pragma once

#include <vide/base_class.hpp>
#include <vide/binary_data.hpp>
#include <vide/binary_serializable_type.hpp>
#include <vide/deferred_data.hpp>
#include <vide/details/validation.hpp>
#include <vide/exception.hpp>
#include <vide/size_tag.hpp>
#include <vide/traits/underlying_archive.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>


namespace vide {

// -------------------------------------------------------------------------------------------------

/// Base of proxy archives, forwards everything to Ar. CRTP, the derived proxy, intercepts values and tags with
/// process_as overloads of its own; serializers see CRTP as their archive
template <typename CRTP, typename Ar>
struct ProxyArchive {
public:
	static constexpr bool enforce_validation = Ar::enforce_validation;
	static constexpr bool is_proxy = true;
	static constexpr bool is_output = Ar::is_output;
	static constexpr bool is_input = Ar::is_input;
	static constexpr bool is_text_archive = Ar::is_text_archive;
	static constexpr bool is_binary_archive = Ar::is_binary_archive;

	template <typename T>
	static constexpr bool supports_binary = Ar::template supports_binary<T>;

public:
	using underlying_archive = underlying_archive_t<Ar>;
	using size_type = typename Ar::size_type;

private:
	Ar& ar;

public:
	explicit constexpr inline ProxyArchive(Ar& ar) : ar(ar) {}

public:
	// Same parameter shape as the archives (const on output) on purpose: a CRTP overload for a
	// specific type is more specialized and wins. A forwarding reference would win instead
	template <typename As, typename T>
	inline void process_as(As& as, const T& var) requires is_output {
		ar.process_as(as, var);
	}

	template <typename As, typename T>
	inline void process_as(As& as, T& var) requires is_input {
		ar.process_as(as, var);
	}

	inline void setNextName(const char* name) {
		ar.setNextName(name);
	}

public:
	template <typename T, typename... Validators>
	inline CRTP& operator()(T&& var, const Validators&... validators) {
		auto& as = static_cast<CRTP&>(*this);
		if constexpr (CRTP::enforce_validation && is_output)
			(validators(var), ...); // Output archive check before save
		as.process_as(as, var);
		if constexpr (CRTP::enforce_validation && is_input)
			(validators(var), ...); // Input archive check after load
		return as;
	}

	template <typename T, typename... Validators>
	inline CRTP& nvp(const char* name, T&& var, const Validators&... validators) {
		auto& as = static_cast<CRTP&>(*this);
		if constexpr (CRTP::enforce_validation && is_output)
			(validators(var), ...); // Output archive check before save
		ar.setNextName(name);
		as.process_as(as, var);
		if constexpr (CRTP::enforce_validation && is_input)
			(validators(var), ...); // Input archive check after load
		return as;
	}

	template <typename Base, typename Derived>
	inline CRTP& base_class(const Derived* derivedThis) {
		auto& as = static_cast<CRTP&>(*this);
		BaseClass<Base> tag(derivedThis);
		as.process_as(as, tag);
		return as;
	}

	template <typename Base, typename Derived>
	inline CRTP& virtual_base_class(const Derived* derivedThis) {
		auto& as = static_cast<CRTP&>(*this);
		VirtualBaseClass<Base> tag(derivedThis);
		as.process_as(as, tag);
		return as;
	}

	template <typename T>
	inline CRTP& defer(T&& var) requires (is_output || std::is_lvalue_reference_v<T>) {
		auto& as = static_cast<CRTP&>(*this);
		DeferredData<T> tag{std::forward<T>(var)};
		as.process_as(as, tag);
		return as;
	}

	template <binary_serializable_type T>
	inline CRTP& binary_data(const T* data, std::size_t size) requires (is_output && is_binary_archive) {
		auto& as = static_cast<CRTP&>(*this);
		BinaryData<sizeof(T)> tag{const_cast<T*>(data), size};
		as.process_as(as, tag);
		return as;
	}

	template <binary_serializable_type T>
	inline CRTP& binary_data(T* data, std::size_t size) requires (is_input && is_binary_archive && !std::is_const_v<T>) {
		auto& as = static_cast<CRTP&>(*this);
		BinaryData<sizeof(T)> tag{data, size};
		as.process_as(as, tag);
		return as;
	}

	template <typename T, typename... Validators>
	inline CRTP& ignore(const Validators&... validators) {
		T var;
		return (*this)(var, validators...);
	}

	template <typename T, typename... Validators>
	inline CRTP& nvp_ignore(const char* name, const Validators&... validators) {
		T var;
		return nvp(name, var, validators...);
	}

	template <typename T, typename... Validators>
	[[nodiscard]] inline T load(const Validators&... validators) requires is_input {
		T var;
		(*this)(var, validators...);
		return var;
	}

	template <typename T, typename... Validators>
	[[nodiscard]] inline T nvp_load(const char* name, const Validators&... validators) requires is_input {
		T var;
		nvp(name, var, validators...);
		return var;
	}

	inline CRTP& size_tag(std::uint64_t size) requires is_output {
		auto& as = static_cast<CRTP&>(*this);
		as.process_as(as, SizeTag<size_type>{detail::checked_size<size_type>(size)});
		return as;
	}
	inline CRTP& size_tag(size_type& size) requires is_input {
		auto& as = static_cast<CRTP&>(*this);
		SizeTag<size_type&> tag{size};
		as.process_as(as, tag);
		return as;
	}
	[[nodiscard]] inline size_type size_tag() requires is_input {
		size_type size{};
		size_tag(size);
		return size;
	}

	template <typename T>
	[[nodiscard]] inline std::size_t safe_to_reserve(size_type numElements) requires is_input {
		return ar.template safe_to_reserve<T>(numElements);
	}

	template <typename T>
	inline void validate_read_size(size_type numElements) requires (is_input && is_binary_archive) {
		ar.template validate_read_size<T>(numElements);
	}

	[[nodiscard]] inline std::size_t maximumBinaryReadSize() const requires (is_input && is_binary_archive) {
		return ar.maximumBinaryReadSize();
	}

public:
	[[nodiscard]] inline auto registerSmartPointer(const std::uint32_t id) requires is_input {
		return ar.registerSmartPointer(id);
	}

	[[nodiscard]] inline auto registerSmartPointer(const void* objectAddress) requires is_output {
		return ar.registerSmartPointer(objectAddress);
	}

	[[nodiscard]] inline auto registerPolymorphicType(std::uint32_t polymorphic_id) requires is_input {
		return ar.registerPolymorphicType(polymorphic_id);
	}

	[[nodiscard]] inline auto registerPolymorphicType(const char* name) requires is_output {
		return ar.registerPolymorphicType(name);
	}

	inline void serializeDeferments() {
		ar.serializeDeferments();
	}

	[[nodiscard]] inline underlying_archive& underlying() noexcept {
		if constexpr (Ar::is_proxy)
			return ar.underlying();
		else
			return ar;
	}

public:
	// Dependent name providers:
	using Exception = vide::Exception;

	[[no_unique_address]] notnull_t notnull;
	[[no_unique_address]] notnullrange_t notnullrange;
	[[no_unique_address]] notempty_t notempty;
	[[nodiscard]] constexpr inline maxsize_t maxsize(std::size_t limit) {
		return maxsize_t{limit};
	}
	[[nodiscard]] constexpr inline minsize_t minsize(std::size_t limit) {
		return minsize_t{limit};
	}
	template <typename Base>
	[[nodiscard]] constexpr inline auto indirect(Base&& baseValidator) {
		return indirect_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
	}
	[[nodiscard]] constexpr inline auto indirect_maxsize(std::size_t limit) {
		return indirect_t<maxsize_t>{limit};
	}
	template <typename Base>
	[[nodiscard]] constexpr inline auto ranged(Base&& baseValidator) {
		return ranged_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
	}
	inline void verify(bool pass, std::string_view message) const {
		if constexpr (CRTP::enforce_validation)
			if (!pass)
				throw Exception("Validation failed during serialization: " + std::string(message));
	}
};

// -------------------------------------------------------------------------------------------------

} // namespace vide
