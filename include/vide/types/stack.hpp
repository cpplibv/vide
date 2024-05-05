#pragma once

#include <stack>

#include <vide/macros.hpp>

// The default container for stack is deque, so let's include that too
#include <vide/types/deque.hpp>


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

//! Saving for std::stack
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::stack<T, C>& stack) {
	ar.nvp("container", stack_detail::container(stack));
}

//! Loading for std::stack
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::stack<T, C>& stack) {
	C container;
	ar.nvp("container", container);
	stack = std::stack<T, C>(std::move(container));
}

} // namespace vide --------------------------------------------------------------------------------
