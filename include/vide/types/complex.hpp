#pragma once

#include <complex>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

//! Serializing (save) for std::complex
template <class Archive, class T>
inline void VIDE_SAVE_FUNCTION_NAME(Archive& ar, const std::complex<T>& comp) {
	ar(VIDE_NVP_("real", comp.real()));
	ar(VIDE_NVP_("imag", comp.imag()));
}

//! Serializing (load) for std::complex
template <class Archive, class T>
inline void VIDE_LOAD_FUNCTION_NAME(Archive& ar, std::complex<T>& bits) {
	T real, imag;
	ar(VIDE_NVP_("real", real));
	ar(VIDE_NVP_("imag", imag));
	bits = {real, imag};
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
