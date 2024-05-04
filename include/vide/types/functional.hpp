#pragma once

#include <functional>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::less
template <class Archive, class T>
inline void serialize(Archive&, std::less<T>&) {
}

} // namespace vide --------------------------------------------------------------------------------
