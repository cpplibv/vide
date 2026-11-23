//

#pragma once

#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>

#include <doctest.h>

#include "proxy_test_group.hpp"


// Each macro instantiates Function<IArchive, OArchive>() as one test case per archive pair:
// CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("vector", test_vector) -> "binary_vector", "portable_binary_vector"

#define CREATE_TEST_CASES_FOR_BINARY_ARCHIVE(Name, Function)                                                           \
	TEST_CASE("binary_" Name) {                                                                                        \
		Function<vide::BinaryInputArchive, vide::BinaryOutputArchive>();                                               \
	}                                                                                                                  \
	TEST_CASE("portable_binary_" Name) {                                                                               \
		Function<vide::PortableBinaryInputArchive, vide::PortableBinaryOutputArchive>();                               \
	}

#define CREATE_TEST_CASES_FOR_TEXT_ARCHIVE(Name, Function)                                                             \
	TEST_CASE("xml_" Name) {                                                                                           \
		Function<vide::XMLInputArchive, vide::XMLOutputArchive>();                                                     \
	}                                                                                                                  \
	TEST_CASE("json_" Name) {                                                                                          \
		Function<vide::JSONInputArchive, vide::JSONOutputArchive>();                                                   \
	}

#define CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_BINARY(Name, Function)                                                \
	TEST_CASE("proxy binary_" Name) {                                                                                  \
		Function<ProxyTestGroup<vide::BinaryInputArchive>, ProxyTestGroup<vide::BinaryOutputArchive>>();               \
	}                                                                                                                  \
	TEST_CASE("proxy portable_binary_" Name) {                                                                         \
		Function<ProxyTestGroup<vide::PortableBinaryInputArchive>, ProxyTestGroup<vide::PortableBinaryOutputArchive>>(); \
	}

#define CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_TEXT(Name, Function)                                                  \
	TEST_CASE("proxy xml_" Name) {                                                                                     \
		Function<ProxyTestGroup<vide::XMLInputArchive>, ProxyTestGroup<vide::XMLOutputArchive>>();                     \
	}                                                                                                                  \
	TEST_CASE("proxy json_" Name) {                                                                                    \
		Function<ProxyTestGroup<vide::JSONInputArchive>, ProxyTestGroup<vide::JSONOutputArchive>>();                   \
	}

#define CREATE_TEST_CASES_FOR_ALL_ARCHIVE(Name, Function)                                                              \
	CREATE_TEST_CASES_FOR_BINARY_ARCHIVE(Name, Function)                                                               \
	CREATE_TEST_CASES_FOR_TEXT_ARCHIVE(Name, Function)                                                                 \
	CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_BINARY(Name, Function)                                                    \
	CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_TEXT(Name, Function)
