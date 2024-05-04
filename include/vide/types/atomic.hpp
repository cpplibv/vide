#pragma once

#include <atomic>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serializing (save) for std::atomic
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::atomic<T> const& a) {
	ar(VIDE_NVP_("atomic_data", a.load()));
}

//! Serializing (load) for std::atomic
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::atomic<T>& a) {
	T tmp;
	ar(VIDE_NVP_("atomic_data", tmp));
	a.store(tmp);
}

} // namespace vide --------------------------------------------------------------------------------
