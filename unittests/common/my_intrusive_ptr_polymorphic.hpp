//

#pragma once

#include <type_traits>

#include "my_intrusive_ptr.hpp"


namespace my { // ----------------------------------------------------------------------------------

#define UNREGISTERED_POLYMORPHIC_EXCEPTION(LoadSave, Name, ArName)                                                            \
	throw vide::Exception("Trying to " LoadSave " an unregistered polymorphic type: " + Name + " with " + ArName + ".\n"      \
			"Make sure your type is registered with VIDE_REGISTER_TYPE and that the archive "                                 \
			"you are using was included (and registered with VIDE_REGISTER_ARCHIVE) prior to calling VIDE_REGISTER_TYPE.\n"   \
			"If your type is already registered and you still see this error, you may need to use VIDE_REGISTER_DYNAMIC_INIT.");

/// Saving my::intrusive_ptr for non-polymorphic types
template <class Archive, class T>
		requires (std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const my::intrusive_ptr<T>& var) {
	if (!var) {
		ar.nvp("ref", std::uint32_t{0});
		return;
	}

	const std::type_info& objectType = typeid(*var);
	static const std::type_info& savingType = typeid(T);

	// objectType can never match savingType if T is abstract (can't have an instance of an abstract class)
	if constexpr (!std::is_abstract_v<T>) {
		if (objectType == savingType) {
			// Matching type_info means this pointer does not need to be cast with our polymorphic machinery
			// as the current T (static type) matched the variable real (dynamic) type. We indicate this with
			// a zero as polymorphic_id.
			const auto objectAddress = var.get();
			const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);
			ar.nvp("ref", ref);
			if (!new_)
				return;

			stored.assign(var);
			ar.nvp("polymorphic_id", vide::polymorphic_id_t{0});
			ar.nvp("data", *var);
			return;
		}
	}

	const auto& bindingMap = vide::detail::getBindingMapOutput<Archive>();
	auto binding = bindingMap.find(std::type_index(objectType));
	if (binding == bindingMap.end()) {
		if constexpr (std::is_abstract_v<T>)
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
		else
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic not abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
	}

	const auto objectAddress = binding->second.downcast(var.get(), savingType);
	const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);

	ar.nvp("ref", ref);
	if (!new_)
		return;

	stored.assign(var); // Aliasing constructor
	binding->second.generic_ptr(&to_underlying_ar(ar), objectAddress);
}

/// Loading my::intrusive_ptr for non-polymorphic types
template <class Archive, class T>
		requires (std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, my::intrusive_ptr<T>& var) {
	using NonConstT = std::remove_const_t<T>;
	static const std::type_info& loadingType = typeid(T);

	std::uint32_t ref;
	ar.nvp("ref", ref);
	if (ref == 0) {
		var = nullptr;
		return;
	}

	struct Storage {
		void* objectAddress;
		my::intrusive_ptr<NonConstT> owner;
	};

	const auto [stored, new_] = ar.registerSmartPointer(ref);
	if (new_) {
		vide::polymorphic_id_t polymorphic_id;
		ar.nvp("polymorphic_id", polymorphic_id);

		if (polymorphic_id == 0) {
			// Zero polymorphic_id means this pointer does not need to be cast with our polymorphic machinery
			// as the current T (static type) matched the variable real (dynamic) type

			if constexpr (std::is_abstract_v<T> || !vide::traits::is_default_constructible<T>) {
				// This case is for when we can't actually construct the shared pointer. Normally this would be caught
				// as the pointer itself is serialized, but since this is a polymorphic pointer, if we tried to serialize
				// the pointer we'd end up back here recursively.  So we have to catch the error here as well, if
				// this was a polymorphic type serialized by its proper pointer type
				throw vide::Exception("Cannot load a polymorphic type '" + vide::util::demangle(loadingType.name()) + "' that is not default constructable");
			} else {
				my::intrusive_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
				NonConstT* objectAddress = ptr.get();
				var = ptr;
				stored.assign(Storage{objectAddress, std::move(ptr)}, loadingType, &type_tag_my_intrusive_ptr,
						+[](void* varPtr, const std::type_info& futureLoadingType) -> void* {
							return vide::detail::PolymorphicCasters::upcast<NonConstT>(static_cast<NonConstT*>(varPtr), futureLoadingType);
						});
				ar.nvp("data", *objectAddress);
				return;
			}
		} else {
			auto [serializer, newType] = ar.registerPolymorphicType(polymorphic_id);
			if (newType) {
				std::string polymorphic_name;
				ar.nvp("polymorphic_name", polymorphic_name);
				serializer = vide::polymorphic_detail::getPolymorphicInputSerializer(ar, polymorphic_name);
			}

			serializer.generic_ptr(&to_underlying_ar(ar), loadingType, [&](void* objectAddress, void* varPtr, const std::type_index& objectType) {
				// Info and upcast must be set before the actual serialization
				// registerFn takes ownership of the loaded pointer
				// We create the shared_ptr by pointing it to varPtr, but with aliasing constructor we store
				// the real (most downcasted / dynamic type) ptr.
				auto ptr = my::intrusive_ptr<NonConstT>(static_cast<NonConstT*>(varPtr));
				var = ptr;
				stored.assign(Storage{objectAddress, std::move(ptr)}, objectType, &type_tag_my_intrusive_ptr, serializer.upcast);
			});
			return;
		}
	}

	if (stored.pointerType != &type_tag_my_intrusive_ptr)
		throw vide::Exception(
				"Type mismatch. Polymorphic pointer type '" + std::string(type_tag_my_intrusive_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as a different '" +
				std::string(stored.pointerType->name) + "' pointer type.");

	if (stored.objectType == loadingType) {
		Storage tmp;
		stored.copyTo(&tmp);
		var = tmp.owner;
	} else {
		if (stored.upcast == nullptr)
			throw vide::Exception(
					"Type mismatch. Polymorphic '" + std::string(type_tag_my_intrusive_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as non-polymorphic type '" +
					vide::util::demangle(stored.objectType.name()) + "', but now it is requested as polymorphic type '" + vide::util::demangledName<T>() + "'.");

		Storage tmp;
		stored.copyTo(&tmp);
		var = my::intrusive_ptr<T>(static_cast<NonConstT*>(stored.upcast(tmp.objectAddress, loadingType)));
	}
}

} // namespace my ----------------------------------------------------------------------------------

struct TestRefCountedPolymorphic {
	mutable int ref_count = 0;
	int value = 0;

public:
	TestRefCountedPolymorphic() = default;
	virtual ~TestRefCountedPolymorphic() = default;
	explicit TestRefCountedPolymorphic(int value) : value(value) {
	}

	template <typename Archive>	void serialize(Archive& ar) {
		ar.nvp("value", value);
	}

	[[nodiscard]] bool operator==(const TestRefCountedPolymorphic& other) const {
		return value == other.value;
	}
};

// -------------------------------------------------------------------------------------------------
