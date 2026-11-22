#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <complex>


namespace vide { // --------------------------------------------------------------------------------

template <class T>
struct serializer<std::complex<T>> {
	template <class Archive>
	static void serialize(Archive& ar, std::complex<T>& comp, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.nvp("real", comp.real());
			ar.nvp("imag", comp.imag());
		} else {
			T real, imag;
			ar.nvp("real", real);
			ar.nvp("imag", imag);
			comp = {real, imag};
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
