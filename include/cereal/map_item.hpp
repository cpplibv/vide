#pragma once

#include <type_traits>
#include <utility>

#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! A wrapper around a key and value for serializing data into maps.
/*! This class just provides a grouping of keys and values into a struct for
	human readable archives. For example, XML archives will use this wrapper
	to write maps like so:

	@code{.xml}
	<mymap>
	  <item0>
		<key>MyFirstKey</key>
		<value>MyFirstValue</value>
	  </item0>
	  <item1>
		<key>MySecondKey</key>
		<value>MySecondValue</value>
	  </item1>
	</mymap>
	@endcode

	\sa make_map_item
	@internal */
template <class Key, class Value>
struct MapItem {
	using KeyType = std::conditional_t<
			std::is_lvalue_reference_v<Key>,
			Key,
			std::decay_t<Key>>;

	using ValueType = std::conditional_t<
			std::is_lvalue_reference_v<Value>,
			Value,
			std::decay_t<Value>>;

	//! Construct a MapItem from a key and a value
	/*! @internal */
	MapItem(Key&& key_, Value&& value_) : key(std::forward<Key>(key_)), value(std::forward<Value>(value_)) {}

	MapItem& operator=(MapItem const&) = delete;

	KeyType key;
	ValueType value;

	//! Serialize the MapItem with the NVPs "key" and "value"
	template <class Archive>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar) {
		ar(CEREAL_NVP(key));
		ar(CEREAL_NVP(value));
	}
};

// -------------------------------------------------------------------------------------------------

//! Create a MapItem so that human readable archives will group keys and values together
/*! @internal
	@relates MapItem */
template <class KeyType, class ValueType> inline
MapItem<KeyType, ValueType> make_map_item(KeyType&& key, ValueType&& value) {
	return {std::forward<KeyType>(key), std::forward<ValueType>(value)};
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
