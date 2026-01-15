//

#pragma once

#include <vide/exception.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! @internal
template <class Archive, class SetT>
inline void implementation_unordered_set_save(Archive& ar, const SetT& set) {
	ar.size_tag(set.size());

	for (const auto& i : set)
		ar(i);
}

//! @internal
template <class Archive, class SetT>
inline void implementation_unordered_set_load(Archive& ar, SetT& set) {
	const auto size = ar.size_tag();
	const auto reservable = ar.template safe_to_reserve<typename SetT::value_type>(size);

	set.clear();
	set.reserve(reservable);

	for (typename Archive::size_type i = 0; i < size; ++i) {
		typename SetT::key_type key;

		ar(key);
		set.emplace(std::move(key));
	}

	if (set.size() != size)
		throw Exception("Failed to load container with unique key constraint. Found " + std::to_string(size - set.size()) + " duplicate key.");
}

} // namespace vide --------------------------------------------------------------------------------
