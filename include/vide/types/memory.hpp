//

#pragma once

#include <vide/access.hpp>
#include <vide/details/util.hpp>
#include <vide/exception.hpp>
#include <vide/serializer.hpp>
#include <vide/smart_ptr_tag.hpp>

#include <cstdint>
#include <memory>


namespace vide { // --------------------------------------------------------------------------------

constexpr inline SmartPtrTag type_tag_std_shared_ptr{"std::shared_ptr"};

template <class T>
		requires (!std::is_polymorphic_v<T>)
struct serializer<std::shared_ptr<T>> {
	template <class Archive>
	static void serialize(Archive& ar, std::shared_ptr<T>& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			if (!var) {
				ar.nvp("ref", std::uint32_t{0});
				return;
			}

			const auto* objectAddress = var.get();
			const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);
			ar.nvp("ref", ref);
			if (new_) {
				stored.assign(var);
				ar.nvp("data", *objectAddress);
			}
		} else {
			std::uint32_t ref;
			ar.nvp("ref", ref);
			if (ref == 0) {
				var = nullptr;
				return;
			}

			const auto [stored, new_] = ar.registerSmartPointer(ref);
			if (new_) {
				using NonConstT = std::remove_const_t<T>;
				// TODO P2: switch to make_shared_for_overwrite
				//		auto realPtr = std::make_shared<Storage<NonConstT>>();
				//		::vide::access::construct<T>(realPtr.get());
				//		auto ptr = std::shared_ptr<NonConstT>(realPtr, reinterpret_cast<NonConstT>(realPtr.get().data));
				std::shared_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
				NonConstT* objectAddress = ptr.get();
				stored.assign(ptr, typeid(T), &type_tag_std_shared_ptr);
				var = std::move(ptr);
				ar.nvp("data", *objectAddress);
				return;
			}

			if (stored.pointerType != &type_tag_std_shared_ptr)
				throw Exception(
						"Type mismatch. Non-polymorphic pointer type '" + std::string(type_tag_std_shared_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as a different '" +
						std::string(stored.pointerType->name) + "' pointer type.");

			if (stored.objectType != typeid(T)) // This check also handles any polymorphic mismatch
				throw Exception(
						"Type mismatch. Non-polymorphic '" + std::string(type_tag_std_shared_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as type '" +
						util::demangle(stored.objectType.name()) + "' is now requested as type '" + util::demangledName<T>() + "'.");

			stored.copyTo(&var);
		}
	}
};

// -------------------------------------------------------------------------------------------------

template <class T, class D>
		requires (!std::is_polymorphic_v<T>)
struct serializer<std::unique_ptr<T, D>> {
	template <class Archive>
	static void serialize(Archive& ar, std::unique_ptr<T, D>& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			// unique_ptr gets one bool of metadata which signifies whether they were a nullptr
			ar.nvp("valid", var != nullptr);
			if (var == nullptr)
				return;

			ar.nvp("data", *var);
		} else {
			bool valid;
			ar.nvp("valid", valid);

			if (!valid) {
				var = nullptr;
				return;
			}

			using NonConstT = std::remove_const_t<T>;
			std::unique_ptr<NonConstT, D> ptr(::vide::access::construct<NonConstT>());
			ar.nvp("data", *ptr);
			var = std::move(ptr);
		}
	}
};

// -------------------------------------------------------------------------------------------------

/// Stored as the std::shared_ptr it points to, for both polymorphic and non-polymorphic types
template <class T>
struct serializer<std::weak_ptr<T>> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, std::weak_ptr<T>& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(var.lock());
		} else {
			std::shared_ptr<T> value;
			ar(value);
			var = std::move(value);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
