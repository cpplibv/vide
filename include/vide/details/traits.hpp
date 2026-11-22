#pragma once

#include <vide/access.hpp>


namespace vide::traits { // ------------------------------------------------------------------------

template <class T>
concept is_default_constructible = requires { vide::access::construct<T>(); };

} // namespace vide::traits ------------------------------------------------------------------------
