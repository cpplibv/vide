//

#pragma once

#include <vide/macros.hpp>
#include <vide/types/implementation_set.hpp>

#include <boost/container/flat_set.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const boost::container::flat_set<K, C, A>& set) {
	implementation_set_save(ar, set);
}

template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, boost::container::flat_set<K, C, A>& set) {
	implementation_set_load(ar, set);
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
