//

#pragma once

// pro
#include <cereal/traits/underlying_archive.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

template <typename CRTP, typename Ar>
struct ProxyArchive {
public:
	static constexpr bool ignores_nvp = Ar::ignores_nvp;
	static constexpr bool is_proxy = true;
	static constexpr bool is_output = Ar::is_output;
	static constexpr bool is_input = Ar::is_input;
	static constexpr bool is_text_archive = Ar::is_text_archive;

	template <typename T>
	static constexpr bool could_serialize = Ar::template could_serialize<T>;

	using underlying_archive = underlying_archive_t<Ar>;

private:
	Ar& ar;

public:
	explicit constexpr inline ProxyArchive(Ar& ar) : ar(ar) {}

public:
	template <typename As, typename T>
	inline CRTP& process_as(As& as, T&& var) {
		ar.process_as(as, var);
		return static_cast<CRTP&>(*this);
	}

public:
	template <typename T>
	inline CRTP& operator()(T&& var) {
		return process_as(static_cast<CRTP&>(*this), std::forward<T>(var));
	}

	template <typename T>
	inline CRTP& operator&(T&& var) {
		return process_as(static_cast<CRTP&>(*this), std::forward<T>(var));
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

	template <class T>
	inline void prologue(T&& t) {
		ar.prologue(std::forward<T>(t));
	}

	template <class T>
	inline void epilogue(T&& t) {
		ar.epilogue(std::forward<T>(t));
	}
};

// -------------------------------------------------------------------------------------------------

} // namespace cereal
