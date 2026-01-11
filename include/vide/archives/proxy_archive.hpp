//

#pragma once

#include <vide/details/validation.hpp>
#include <vide/exception.hpp>
#include <vide/nvp.hpp>
#include <vide/traits/underlying_archive.hpp>

#include <utility>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename CRTP, typename Ar>
struct ProxyArchive {
public:
	static constexpr bool ignores_nvp = Ar::ignores_nvp;
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
	template <typename As, typename T>
	inline void process_as(As& as, T&& var) {
		ar.process_as(as, var);
	}

public:
	template <typename T, typename... Validators>
	inline CRTP& operator()(T&& var, const Validators&... validators) {
		auto& as = static_cast<CRTP&>(*this);
		if constexpr (CRTP::enforce_validation && is_output)
			(validators(var), ...); // Output archive check before save
		as.process_as(as, std::forward<T>(var));
		if constexpr (CRTP::enforce_validation && is_input)
			(validators(var), ...); // Input archive check after load
		return as;
	}

	template <typename T, typename... Validators>
	inline CRTP& nvp(const char* name, T&& var, const Validators&... validators) {
		if constexpr (ignores_nvp)
			return (*this)(std::forward<T>(var), validators...);
		else
			return (*this)(::vide::make_nvp(name, std::forward<T>(var)), validators...);
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

	inline CRTP& size_tag(uint32_t size) requires is_output {
		ar.size_tag(size);
		return static_cast<CRTP&>(*this);
	}
	inline CRTP& size_tag(uint64_t size) requires is_output {
		ar.size_tag(size);
		return static_cast<CRTP&>(*this);
	}
	inline CRTP& size_tag(size_type& size) requires is_input {
		ar.size_tag(size);
		return static_cast<CRTP&>(*this);
	}
	inline size_type size_tag() requires is_input {
		return ar.size_tag();
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
	inline void registerSharedPointer(const std::uint32_t id, std::shared_ptr<void> ptr) {
		ar.registerSharedPointer(id, std::move(ptr));
	}

	[[nodiscard]] inline std::uint32_t registerSharedPointer(const std::shared_ptr<const void>& ptr) {
		return ar.registerSharedPointer(ptr);
	}

	[[nodiscard]] inline std::shared_ptr<void> getSharedPointer(const std::uint32_t id) {
		return ar.getSharedPointer(id);
	}

	inline void serializeDeferments() {
		ar.serializeDeferments();
	}

	inline void registerPolymorphicName(const std::uint32_t id, const std::string& name) {
		ar.registerPolymorphicName(id, name);
	}

	[[nodiscard]] inline std::string getPolymorphicName(const std::uint32_t id) {
		return ar.getPolymorphicName(id);
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
	[[no_unique_address]] notempty_t notempty;
	[[nodiscard]] constexpr inline maxsize_t maxsize(std::size_t limit) {
		return maxsize_t{limit};
	}
	inline void verify(bool pass, std::string_view message) const {
		if constexpr (CRTP::enforce_validation)
			if (!pass)
				throw Exception("Validation failed during serialization: " + std::string(message));
	}
};

// -------------------------------------------------------------------------------------------------

} // namespace vide
