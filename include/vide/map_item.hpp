#pragma once

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

/// Tag of one map entry: text archives group its key and value, {"key": "a", "value": 1} in JSON.
/// Key and Value are const when saving
template <class Key, class Value>
struct MapItem {
	Key& key;
	Value& value;

	template <class Archive>
	inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar) {
		ar.nvp("key", key);
		ar.nvp("value", value);
	}
};

} // namespace vide --------------------------------------------------------------------------------
