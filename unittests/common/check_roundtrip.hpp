//

#pragma once

#include <source_location>
#include <string>

#include <doctest/doctest.h>

#include "roundtrip.hpp"
#include "sample_structs.hpp"
#include "serialization_calls.hpp"


/// Checks that every object of the sample structs and of Instrumented in value ran its own save and load exactly once
/// since log opened
template <typename... Instrumented>
void check_once_each(const CallLog& log, const auto& value) {
	log.check<StructInternalSerialize>(count_instances<StructInternalSerialize>(value));
	log.check<StructInternalSplit>(count_instances<StructInternalSplit>(value));
	log.check<StructExternalSerialize>(count_instances<StructExternalSerialize>(value));
	log.check<StructExternalSplit>(count_instances<StructExternalSplit>(value));
	(log.check<Instrumented>(count_instances<Instrumented>(value)), ...);
}

/// Round trips value; the loaded value must equal it, and every instrumented object inside must have run its own save
/// and load exactly once: check_roundtrip<Ar>(value), check_roundtrip<Ar, MyType>(value) for a test's own type
template <typename Ar, typename... Instrumented, typename T>
void check_roundtrip(const T& value, std::source_location location = std::source_location::current()) {
	INFO("check_roundtrip at ", std::string(location.file_name()), ":", location.line());
	const CallLog calls;
	const T loaded = roundtrip<Ar>(value);
	CHECK_EQ(loaded, value);
	check_once_each<Instrumented...>(calls, value);
}
