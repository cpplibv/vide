//

#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <meta>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_set>
#include <variant>
#include <vector>

#include <doctest/doctest.h>
#include <vide/details/util.hpp>


/// Saves and loads counted for one type
struct SerializationCalls {
	std::size_t saves = 0;
	std::size_t loads = 0;

	constexpr bool operator==(const SerializationCalls&) const = default;

	friend std::ostream& operator<<(std::ostream& os, const SerializationCalls& calls) {
		return os << "{saves: " << calls.saves << ", loads: " << calls.loads << "}";
	}
};

/// Records the serialization calls of instrumented types on this thread while alive. Equal values after a round trip
/// cannot show which function did the work (reflection could produce the same data, a skipped load can leave a
/// matching default); the log can. Logs nest, an inner call reaches every open log
class CallLog {
public:
	/// One run of a type's own save or load function
	struct Call {
		std::type_index type;
		const void* object; ///< The S subobject: a virtual base reached twice shows the same address twice
		bool save;

		auto operator<=>(const Call&) const = default;
	};

private:
	static inline thread_local CallLog* innermost = nullptr;

	mutable std::vector<Call> calls; // Mutable: logs are usually const locals, record() still appends
	CallLog* outer;

public:
	CallLog() :
		outer(innermost) {
		innermost = this;
	}
	~CallLog() {
		innermost = outer;
	}
	CallLog(const CallLog&) = delete;
	CallLog& operator=(const CallLog&) = delete;

	static void record(const Call& call) {
		for (CallLog* log = innermost; log != nullptr; log = log->outer)
			log->calls.push_back(call);
	}

	template <typename S>
	[[nodiscard]] SerializationCalls of() const {
		SerializationCalls result;
		for (const Call& call : calls)
			if (call.type == typeid(S))
				++(call.save ? result.saves : result.loads);
		return result;
	}

	/// Checks that S's own save and load each ran count times
	template <typename S>
	void check(std::size_t count) const {
		INFO("calls of ", vide::util::demangledName<S>());
		CHECK_EQ(of<S>(), SerializationCalls{count, count});
	}

	/// Checks that no object ran its own save (or load) twice and that every type loaded as many objects as it saved.
	/// Holds only while the serialized objects stay alive: a reused address would look like a repeat
	void check_once_per_object() const {
		std::vector<Call> sorted = calls;
		std::ranges::sort(sorted);
		for (auto repeat = std::ranges::adjacent_find(sorted); repeat != sorted.end();
				repeat = std::ranges::adjacent_find(std::ranges::upper_bound(sorted, *repeat), sorted.end())) {
			INFO("object of ", vide::util::demangle(repeat->type.name()), " at ", repeat->object);
			FAIL_CHECK("repeated ", repeat->save ? "save" : "load");
		}

		std::unordered_set<std::type_index> types;
		for (const Call& call : calls)
			types.insert(call.type);
		for (const std::type_index type : types) {
			const auto count = [&](bool save) { return std::ranges::count_if(calls, [&](const Call& c) { return c.type == type && c.save == save; }); };
			INFO("calls of ", vide::util::demangle(type.name()));
			CHECK_EQ(count(true), count(false));
		}
	}
};

/// The first statement of every serialization function of an instrumented type: record_call<Archive>(this), or
/// record_call<Archive>(&value) in a vide::serializer specialization
template <typename Archive, typename S>
void record_call(const S* object) {
	CallLog::record({typeid(S), object, Archive::is_output});
}

/// The underlying container of std::queue, std::stack or std::priority_queue (their protected member c)
template <typename Adapter>
[[nodiscard]] const typename Adapter::container_type& adapter_container(const Adapter& adapter) {
	struct Exposed : Adapter {
		static const typename Adapter::container_type& container(const Adapter& a) {
			return a.*&Exposed::c;
		}
	};
	return Exposed::container(adapter);
}

namespace detail {

template <typename S, typename T>
void count_instances(const T& value, std::size_t& count, std::unordered_set<const void*>& visited_pointees) {
	if constexpr (std::is_same_v<T, S>) {
		++count; // Instrumented types are leaves; their members are their own business
	} else if constexpr (requires { value.load(); }) { // std::atomic
		count_instances<S>(value.load(), count, visited_pointees);
	} else if constexpr (requires { typename T::container_type; } && !requires { value.begin(); }) {
		count_instances<S>(adapter_container(value), count, visited_pointees);
	} else if constexpr (requires { std::begin(value); }) {
		for (const auto& element : value)
			count_instances<S>(element, count, visited_pointees);
	} else if constexpr (requires { std::tuple_size<T>::value; }) {
		std::apply([&](const auto&... members) { (count_instances<S>(members, count, visited_pointees), ...); }, value);
	} else if constexpr (requires { std::visit([](const auto&) {}, value); }) {
		std::visit([&](const auto& member) { count_instances<S>(member, count, visited_pointees); }, value);
	} else if constexpr (requires { static_cast<bool>(value); *value; } && !std::is_arithmetic_v<T>) {
		// A pointee shared by several pointers counts once
		if (value && visited_pointees.insert(std::addressof(*value)).second)
			count_instances<S>(*value, count, visited_pointees);
	} else if constexpr (std::is_class_v<T> && !std::is_union_v<T>) {
		template for (constexpr auto member : std::define_static_array(
				std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())))
			count_instances<S>(value.[:member:], count, visited_pointees);
	}
}

} // namespace detail

/// Number of S objects in value: as elements, pair / tuple members, optional and variant contents, container adapter
/// elements, pointees (each once) and data members of other classes
template <typename S, typename T>
[[nodiscard]] std::size_t count_instances(const T& value) {
	std::size_t count = 0;
	std::unordered_set<const void*> visited_pointees;
	detail::count_instances<S>(value, count, visited_pointees);
	return count;
}
