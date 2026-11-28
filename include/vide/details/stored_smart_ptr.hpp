#pragma once

#include <vide/details/diagnostic.hpp>
#include <vide/details/polymorphic_helper.hpp>
#include <vide/macros.hpp>
#include <vide/smart_ptr_tag.hpp>

#include <cassert>
#include <cstddef>
#include <string_view>
#include <typeindex>


namespace vide::detail { // ------------------------------------------------------------------------

static constexpr std::size_t SmartPtrMaxSize = VIDE_SMART_PTR_MAX_SIZE;

template <class Pointer>
consteval std::string_view oversized_smart_ptr_message() {
	return diagnostic_message(
			"    ERROR: Vide cannot track the smart pointer [" + diagnostic_name<Pointer>() + "]: it takes " + diagnostic_number(sizeof(Pointer)) +
			" bytes, VIDE_SMART_PTR_MAX_SIZE allows " + diagnostic_number(SmartPtrMaxSize) + ".\n"
			"    Define VIDE_SMART_PTR_MAX_SIZE to at least " + diagnostic_number(sizeof(Pointer)) + ", project wide: every translation unit must agree.\n");
}

// -------------------------------------------------------------------------------------------------

struct StoredInputPointer {
	using DeleteFn = void(*)(void* self);
	using AssignFn = void(*)(void* to, const void* from);

public:
	std::type_index objectType{typeid(void)};
	const SmartPtrTag* pointerType = nullptr;
	polymorphic_detail::InputSerializers::UpcastFn upcast = nullptr;

private:
	alignas(sizeof(void*)) std::byte storage[SmartPtrMaxSize]{};
	AssignFn assignFn = nullptr;
	DeleteFn deleteFn = nullptr;

public:
	StoredInputPointer() = default;
	StoredInputPointer(const StoredInputPointer& rhs) = delete;
	StoredInputPointer(StoredInputPointer&& rhs) = delete;
	StoredInputPointer& operator=(const StoredInputPointer& rhs) = delete;
	StoredInputPointer& operator=(StoredInputPointer&& rhs) = delete;

	template <typename PointerArg>
	void assign(PointerArg&& pointer_, std::type_index objectType_, const SmartPtrTag* pointerType_, polymorphic_detail::InputSerializers::UpcastFn upcast_ = nullptr) {
		using Pointer = std::remove_reference_t<PointerArg>;
		// A branch of its own: the placement new would follow up with a warning
		if constexpr (sizeof(Pointer) > SmartPtrMaxSize) {
			static_assert(false, oversized_smart_ptr_message<Pointer>());
		} else {
			assert(deleteFn == nullptr);

			objectType = objectType_;
			pointerType = pointerType_;
			upcast = upcast_;
			assignFn = +[](void* to, const void* from) {
				*static_cast<Pointer*>(to) = *static_cast<const Pointer*>(from);
			};
			deleteFn = +[](void* self) {
				static_cast<Pointer*>(self)->~Pointer();
			};
			new(storage) Pointer(std::forward<PointerArg>(pointer_));
		}
	}

	void copyTo(void* pointer) {
		assert(assignFn != nullptr);
		assignFn(pointer, storage);
	}

	~StoredInputPointer() {
		assert(deleteFn != nullptr);
		deleteFn(storage);
	}
};

// -------------------------------------------------------------------------------------------------

struct StoredOutputPointer {
	using DeleteFn = void(*)(void* self);
	using AssignFn = void(*)(void* to, const void* from);

public:
	std::uint32_t ref = 0;

private:
	alignas(sizeof(void*)) std::byte storage[SmartPtrMaxSize]{};
	DeleteFn deleteFn = nullptr;

public:
	StoredOutputPointer() = default;
	StoredOutputPointer(const StoredOutputPointer& rhs) = delete;
	StoredOutputPointer(StoredOutputPointer&& rhs) = delete;
	StoredOutputPointer& operator=(const StoredOutputPointer& rhs) = delete;
	StoredOutputPointer& operator=(StoredOutputPointer&& rhs) = delete;

	template <typename PointerArg>
	void assign(PointerArg&& pointer_) {
		using Pointer = std::remove_reference_t<PointerArg>;
		// A branch of its own: the placement new would follow up with a warning
		if constexpr (sizeof(Pointer) > SmartPtrMaxSize) {
			static_assert(false, oversized_smart_ptr_message<Pointer>());
		} else {
			assert(deleteFn == nullptr);

			deleteFn = +[](void* self) {
				static_cast<Pointer*>(self)->~Pointer();
			};
			new(storage) Pointer(std::forward<PointerArg>(pointer_));
		}
	}

	bool isAssigned() const {
		return deleteFn != nullptr;
	}

	~StoredOutputPointer() {
		assert(deleteFn != nullptr);
		deleteFn(storage);
	}
};

} // namespace vide::detail ------------------------------------------------------------------------
