#pragma once

#include <atomic>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

// std::atomic has implicit conversion operator for the underlying type which can find undesired
// serialization functions which leads to ambiguity.
// Provide better overloads than any with implicit conversion and delete it.

template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::atomic<T>& a) = delete;
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::atomic<T>& a) = delete;
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::atomic<T>& a) = delete;

// Serializing (save) for std::atomic
template <class Archive, class T>
inline T VIDE_FUNCTION_NAME_SAVE_MINIMAL(const Archive&, const std::atomic<T>& var) {
	return var.load();
}

//! Serializing (load) for std::atomic
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD_MINIMAL(const Archive&, std::atomic<T>& var, T value) {
	var.store(value);
}

} // namespace vide --------------------------------------------------------------------------------
