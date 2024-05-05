#pragma once

#include <functional>


namespace vide { // --------------------------------------------------------------------------------

template <class Archive, class T>
inline void serialize(Archive&, std::less<T>&) {
}

template <class Archive, class T>
inline void serialize(Archive&, std::less_equal<T>&) {
}

template <class Archive, class T>
inline void serialize(Archive&, std::greater<T>&) {
}

template <class Archive, class T>
inline void serialize(Archive&, std::greater_equal<T>&) {
}

} // namespace vide --------------------------------------------------------------------------------
