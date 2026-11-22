#pragma once

#include <vide/types/std_deque.hpp> // The default container for stack is deque, so let's include that too

#include <cstdint>
#include <stack>


namespace vide { // --------------------------------------------------------------------------------
namespace stack_detail {

//! Allows access to the protected container in stack
template <class T, class C>
inline const C& container(const std::stack<T, C>& stack) {
	struct H : public std::stack<T, C> {
		static const C& get(const std::stack<T, C>& s) {
			return s.*(&H::c);
		}
	};

	return H::get(stack);
}

} // namespace stack_detail ------------------------------------------------------------------------

template <class T, class C>
struct serializer<std::stack<T, C>> {
	template <class Archive>
	static void serialize(Archive& ar, std::stack<T, C>& stack, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.nvp("container", stack_detail::container(stack));
		} else {
			C container;
			ar.nvp("container", container);
			stack = std::stack<T, C>(std::move(container));
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
