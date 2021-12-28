#pragma once

#include <type_traits>

#include <cereal/access.hpp>


namespace cereal {
namespace traits {
namespace detail {

// -------------------------------------------------------------------------------------------------

struct shared_from_this_wrapper {
	template <class U>
	static auto (check)(U const& t) -> decltype(::cereal::access::shared_from_this(t), std::true_type());

	static auto (check)(...) -> decltype(std::false_type());

	template <class U>
	static auto get(U const& t) -> decltype(t.shared_from_this());
};

} // namespace detail ------------------------------------------------------------------------------

//! Determine if T or any base class of T has inherited from std::enable_shared_from_this
template <class T>
struct has_shared_from_this : decltype((detail::shared_from_this_wrapper::check)(std::declval<T>())) {};

//! Get the type of the base class of T which inherited from std::enable_shared_from_this
template <class T>
struct get_shared_from_this_base {
private:
	using PtrType = decltype(detail::shared_from_this_wrapper::get(std::declval<T>()));
public:
	//! The type of the base of T that inherited from std::enable_shared_from_this
	using type = typename std::decay<typename PtrType::element_type>::type;
};

// -------------------------------------------------------------------------------------------------

} // namespace traits
} // namespace cereal
