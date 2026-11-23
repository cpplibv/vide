//

#pragma once

#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <random>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include <doctest/doctest.h>
#include <vide/details/util.hpp>

#include "serialization_calls.hpp"


/// Round trips many values through one archive, each with its own check. Ar is an Archives<Input, Output> pair;
/// exceptions name the source location of the registering call
template <typename Ar>
class Tester {
	using IArchive = typename Ar::input;
	using OArchive = typename Ar::output;

	struct BaseTask {
		std::source_location location;

		explicit BaseTask(std::source_location location) :
			location(location) {}

		virtual ~BaseTask() = default;
		virtual void save(OArchive&) = 0;
		virtual void load(IArchive&) = 0;
		virtual void check() = 0;

		/// Runs step, rethrowing its exception with the task location
		void annotated(std::string_view step, auto&& fn) const {
			try {
				fn();
			} catch (const std::exception& e) {
				throw std::runtime_error(describe(step, vide::util::demangle(typeid(e).name()), e.what()));
			} catch (...) {
				throw std::runtime_error(describe(step, "unknown exception", "?"));
			}
		}

		std::string describe(std::string_view step, std::string_view type, std::string_view message) const {
			std::ostringstream os;
			os << "\n\tIn test task: " << location.file_name() << ":" << location.line() << ":" << location.column()
					<< "\n\tDuring " << step << " test task THROW exception of type [" << type << "] with message:\n\t\t"
					<< message;
			return std::move(os).str();
		}
	};

	std::vector<std::unique_ptr<BaseTask>> tasks;

public:
	template <typename T>
	using CheckFn = std::function<void(const T& saved, const T& loaded)>;

	/// Registers value for the next run(); check receives the saved and the loaded value
	template <typename T>
	void operator()(T&& value, CheckFn<std::remove_cvref_t<T>> check,
			std::source_location location = std::source_location::current()) {
		using Value = std::remove_cvref_t<T>;

		struct Task : BaseTask {
			Value saved;
			Value loaded{};
			CheckFn<Value> check_fn;

			Task(T&& value, CheckFn<Value>&& check, std::source_location location) :
				BaseTask(location),
				saved(std::forward<T>(value)),
				check_fn(std::move(check)) {}

			void save(OArchive& ar) override {
				this->annotated("output serialization", [&] { ar(std::as_const(saved)); });
			}
			void load(IArchive& ar) override {
				this->annotated("input serialization", [&] { ar(loaded); });
			}
			void check() override {
				this->annotated("result evaluation", [&] { check_fn(std::as_const(saved), std::as_const(loaded)); });
			}
		};

		tasks.emplace_back(std::make_unique<Task>(std::forward<T>(value), std::move(check), location));
	}

	/// Saves all values into one archive, loads them back in the same order and runs the checks. Also checks that no
	/// object ran its own save or load twice (CallLog::check_once_per_object())
	void run() {
		const CallLog calls;
		std::ostringstream os;
		{
			OArchive ar(os);
			for (const auto& task : tasks)
				task->save(ar);
		}

		std::istringstream is(std::move(os).str());
		{
			IArchive ar(is);
			for (const auto& task : tasks)
				task->load(ar);
		}

		for (const auto& task : tasks)
			task->check();
		calls.check_once_per_object();
	}

	void shuffle(std::mt19937& rng) {
		std::ranges::shuffle(tasks, rng);
	}
};

/// Repeats register_tasks(tester) + run() for the given rounds; the first and last keep registration order (easier to
/// debug), the rest shuffle it so values meet many different archive states
template <typename Ar>
void run_rounds(int rounds, std::mt19937& rng, auto&& register_tasks) {
	for (int round = 0; round < rounds; ++round) {
		CAPTURE(round);
		Tester<Ar> tester;
		register_tasks(tester);
		if (round != 0 && round != rounds - 1)
			tester.shuffle(rng);
		tester.run();
	}
}

// Common Tester checks for pointers

/// The loaded pointer is null
inline constexpr auto expect_null = [](const auto&, const auto& loaded) {
	CHECK_EQ(loaded.get(), nullptr);
};

/// The loaded pointer owns a new object equal to the saved one
inline constexpr auto expect_equal_copy = [](const auto& saved, const auto& loaded) {
	REQUIRE_NE(loaded.get(), nullptr);
	CHECK_NE(loaded.get(), saved.get());
	CHECK_EQ(*loaded, *saved);
};

/// The loaded pointee equals the saved one
inline constexpr auto expect_equal = [](const auto& saved, const auto& loaded) {
	REQUIRE_NE(loaded.get(), nullptr);
	CHECK_EQ(*loaded, *saved);
};

/// Like expect_equal(), and both also cast to Derived with equal results: shared_as<Derived, Base>() values
template <typename Derived>
inline constexpr auto expect_equal_as = [](const auto& saved, const auto& loaded) {
	REQUIRE_NE(loaded.get(), nullptr);
	CHECK_EQ(*loaded, *saved);
	const auto saved_derived = std::dynamic_pointer_cast<Derived>(saved);
	const auto loaded_derived = std::dynamic_pointer_cast<Derived>(loaded);
	REQUIRE(saved_derived);
	REQUIRE(loaded_derived);
	CHECK_EQ(*loaded_derived, *saved_derived);
};
