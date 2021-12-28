20211224 : 5.41


Type files that still have to be cleaned from cereal/cereal.hpp include 
bitset
base_class
common
memory
polymorphic



//template <class Archive, class T, size_t N>
//inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const std::array<T, N>& array) {
//	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
//	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
//	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;
//
//	if constexpr (serialize_as_binary) {
//		ar(binary_data(array.data(), sizeof(array)));
//
//	} else {
//		for (const auto& i : array)
//			ar(i);
//	}
//}
//
//template <class Archive, class T, size_t N>
//inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::array<T, N>& array) {
//	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
//	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
//	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;
//
//	if constexpr (serialize_as_binary) {
//		ar(binary_data(array.data(), sizeof(array)));
//
//	} else {
//		for (auto& i : array)
//			ar(i);
//	}
//}
