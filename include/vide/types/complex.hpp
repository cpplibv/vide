#pragma once

#include <complex>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serializing (save) for std::complex
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::complex<T>& comp) {
	ar.nvp("real", comp.real());
	ar.nvp("imag", comp.imag());
}

//! Serializing (load) for std::complex
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::complex<T>& bits) {
	T real, imag;
	ar.nvp("real", real);
	ar.nvp("imag", imag);
	bits = {real, imag};
}

} // namespace vide --------------------------------------------------------------------------------
