#pragma once

#include <complex>

#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! Serializing (save) for std::complex
template <class Archive, class T>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const std::complex<T>& comp) {
	ar(CEREAL_NVP_("real", comp.real()));
	ar(CEREAL_NVP_("imag", comp.imag()));
}

//! Serializing (load) for std::complex
template <class Archive, class T>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::complex<T>& bits) {
	T real, imag;
	ar(CEREAL_NVP_("real", real));
	ar(CEREAL_NVP_("imag", imag));
	bits = {real, imag};
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
