#pragma once

#include <chrono>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving std::chrono::duration
template <class Archive, class R, class P>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::chrono::duration<R, P> const& dur) {
	ar.nvp("count", dur.count());
}

//! Loading std::chrono::duration
template <class Archive, class R, class P>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::chrono::duration<R, P>& dur) {
	R count;
	ar.nvp("count", count);
	dur = std::chrono::duration<R, P>{count};
}

//! Saving std::chrono::time_point
template <class Archive, class C, class D>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::chrono::time_point<C, D> const& dur) {
	ar.nvp("time_since_epoch", dur.time_since_epoch());
}

//! Loading std::chrono::time_point
template <class Archive, class C, class D>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::chrono::time_point<C, D>& dur) {
	D elapsed;
	ar.nvp("time_since_epoch", elapsed);
	dur = std::chrono::time_point<C, D>{elapsed};
}

} // namespace vide --------------------------------------------------------------------------------

