#pragma once

#include <vide/details/polymorphic_helper.hpp>
#include <vide/macros.hpp>
#include <vide/smart_ptr_tag.hpp>

#include <cassert>
#include <cstddef>
#include <typeindex>


namespace vide::detail { // ------------------------------------------------------------------------

static constexpr std::size_t SmartPtrMaxSize = VIDE_SMART_PTR_MAX_SIZE;

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
		static_assert(sizeof(Pointer) <= SmartPtrMaxSize);
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
		static_assert(sizeof(Pointer) <= SmartPtrMaxSize);
		assert(deleteFn == nullptr);

		deleteFn = +[](void* self) {
			static_cast<Pointer*>(self)->~Pointer();
		};
		new(storage) Pointer(std::forward<PointerArg>(pointer_));
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
