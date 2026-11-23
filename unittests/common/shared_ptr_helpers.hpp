//

#pragma once

#include <cstddef>
#include <memory>
#include <utility>


// Short spellings for the polymorphic tests, where shared_ptr construction and casts dominate the code

template <typename T>
[[nodiscard]] std::shared_ptr<T> shared(std::nullptr_t) {
	return std::shared_ptr<T>(nullptr);
}

template <typename T, typename... Args>
[[nodiscard]] std::shared_ptr<T> shared(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename Derived, typename Base, typename... Args>
[[nodiscard]] std::shared_ptr<Base> shared_as(Args&&... args) {
	return std::static_pointer_cast<Base>(std::make_shared<Derived>(std::forward<Args>(args)...));
}

template <typename To, typename From>
[[nodiscard]] std::shared_ptr<To> dcast(const std::shared_ptr<From>& ptr) {
	return std::dynamic_pointer_cast<To>(ptr);
}

template <typename To, typename From>
[[nodiscard]] std::shared_ptr<To> scast(const std::shared_ptr<From>& ptr) {
	return std::static_pointer_cast<To>(ptr);
}

template <typename To, typename From>
[[nodiscard]] bool castable_and_equal(const std::shared_ptr<From>& in, const std::shared_ptr<From>& out) {
	const auto o = std::dynamic_pointer_cast<To>(out);
	const auto i = std::dynamic_pointer_cast<To>(in);
	return o && i && *i == *o;
}
