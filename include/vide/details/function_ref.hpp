#pragma once


#include <functional>
#include <utility>


namespace vide { // --------------------------------------------------------------------------------

/// A lightweight non-owning reference to a callable.
template <typename F>
class function_ref;

/// A lightweight non-owning reference to a callable. Specialization for function signature.
template <typename R, typename... Args>
class function_ref<R(Args...)> {
private:
	void* obj_ = nullptr;
	R(*callback_)(void*, Args...) = nullptr;

public:
	constexpr inline function_ref() noexcept = delete;
	constexpr inline function_ref(const function_ref& rhs) noexcept = default;
	explicit(false) constexpr inline function_ref(std::nullptr_t) noexcept {}

	template <typename F>
			requires (!std::is_same_v<std::decay_t<F>, function_ref>) && std::is_invocable_r_v<R, F &&, Args...>
	constexpr inline function_ref(F &&f) noexcept :
		obj_(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(f)))) {
		callback_ = [](void* obj, Args... args) -> R {
			return std::invoke(
					*reinterpret_cast<std::add_pointer_t<F>>(obj),
					std::forward<Args>(args)...);
		};
	}

	constexpr inline function_ref& operator=(const function_ref &rhs) noexcept = default;

	template <typename F>
			requires std::is_invocable_r_v<R, F &&, Args...>
	constexpr inline function_ref& operator=(F &&f) noexcept {
		obj_ = reinterpret_cast<void*>(std::addressof(f));
		callback_ = [](void* obj, Args... args) {
			return std::invoke(
					*reinterpret_cast<std::add_pointer_t<F>>(obj),
					std::forward<Args>(args)...);
		};

		return *this;
	}

	constexpr inline void swap(function_ref &rhs) noexcept {
		std::swap(obj_, rhs.obj_);
		std::swap(callback_, rhs.callback_);
	}

	constexpr inline R operator()(Args... args) const {
		return callback_(obj_, std::forward<Args>(args)...);
	}

	[[nodiscard]] explicit inline operator bool() const noexcept {
		return callback_ != nullptr;
	}
};

template <typename R, typename... Args>
constexpr inline void swap(function_ref<R(Args...)> &lhs, function_ref<R(Args...)> &rhs) noexcept {
	lhs.swap(rhs);
}

template <typename R, typename... Args>
function_ref(R(*)(Args...)) -> function_ref<R(Args...)>;

} // namespace vide --------------------------------------------------------------------------------
