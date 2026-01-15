//

#pragma once

#include <vide/exception.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! @internal
template <class Archive, class SetT>
inline void implementation_set_save(Archive& ar, const SetT& set) {
	ar.size_tag(set.size());

	for (const auto& i : set)
		ar(i);
}

//! @internal
template <class Archive, class SetT>
inline void implementation_set_load(Archive& ar, SetT& set) {
	const auto size = ar.size_tag();

	set.clear();

	auto hint = set.begin();
	for (typename Archive::size_type i = 0; i < size; ++i) {
		typename SetT::key_type key;

		ar(key);
		hint = set.emplace_hint(hint, std::move(key));
	}

	if (set.size() != size)
		throw Exception("Failed to load container with unique key constraint. Found " + std::to_string(size - set.size()) + " duplicate key.");
}

} // namespace vide --------------------------------------------------------------------------------
