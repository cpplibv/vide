//

#pragma once

#include <functional>
#include <source_location>
#include <sstream>
#include <type_traits>


template <class IArchive, class OArchive>
struct Tester {
	struct BaseTask {
		std::source_location testLocation;

		explicit BaseTask(std::source_location&& testLocation) : testLocation(std::move(testLocation)) {}

		virtual void out(OArchive&) = 0;
		virtual void in(IArchive&) = 0;
		virtual void check() = 0;
		virtual ~BaseTask() = default;

		std::string formatExceptionMessage(std::string_view operation, const char* exceptionType, const char* originalMessage) const {
			std::ostringstream os;
			os << "\n";
			os << "\tIn test task: " << testLocation.file_name() << ":" << testLocation.line() << ":" << testLocation.column() << "\n";
			os << "\tDuring " << operation << " test task THROW exception of type [" << vide::util::demangle(exceptionType) << "] with message:\n";
			os << "\t\t" << originalMessage;
			return std::move(os).str();
		}
	};

	std::vector<std::unique_ptr<BaseTask>> tasks;

	template <typename T>
	using CheckFn = std::function<void(const std::remove_cvref_t<T>&, const std::remove_cvref_t<T>&)>;

	template <typename T>
	void operator()(T&& output, CheckFn<T> checkFn, std::source_location testLocation = std::source_location::current()) {
		struct Task : BaseTask {
			std::remove_cvref_t<T> output;
			std::remove_cvref_t<T> input;
			CheckFn<T> checkFn;

			explicit Task(T&& output, CheckFn<T>&& checkFn, std::source_location&& testLocation) :
				BaseTask{std::move(testLocation)},
				output(std::forward<T>(output)),
				checkFn(std::move(checkFn)) {
			}

			virtual void out(OArchive& oar) override {
				try {
					oar(std::as_const(output));
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("output serialization", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("output serialization", "unhandled exception", "?"));
				}
			}
			virtual void in(IArchive& iar) override {
				try {
					iar(input);
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("input serialization", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("input serialization", "unhandled exception", "?"));
				}
			}
			virtual void check() override {
				try {
					checkFn(std::as_const(output), std::as_const(input));
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("result evaluation", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("result evaluation", "unhandled exception", "?"));
				}
			}
		};

		tasks.emplace_back(std::make_unique<Task>(std::forward<T>(output), std::move(checkFn), std::move(testLocation)));
	}

	void outputAll(OArchive& oar) {
		for (const auto& task : tasks)
			task->out(oar);
	}
	void inputAll(IArchive& iar) {
		for (const auto& task : tasks)
			task->in(iar);
	}
	void checkAll() {
		for (const auto& task : tasks)
			task->check();
	}
	void shuffle(std::mt19937& rng) {
		std::ranges::shuffle(tasks, rng);
	}
};

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
