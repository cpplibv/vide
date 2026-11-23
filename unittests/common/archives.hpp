//

#pragma once

#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>

#include <doctest/doctest.h>

#include "proxy_test_group.hpp"


/// Input and output archive of one format; the type parameter of the archive test cases
template <typename Input, typename Output>
struct Archives {
	using input = Input;
	using output = Output;
};

/// Same format, both sides wrapped: proxied<BinaryArchives, NonValidatingProxy> -> Archives<NonValidatingProxy<...>, ...>
template <typename Ar, template <typename> typename Proxy>
using proxied = Archives<Proxy<typename Ar::input>, Proxy<typename Ar::output>>;

using BinaryArchives = Archives<vide::BinaryInputArchive, vide::BinaryOutputArchive>;
using PortableBinaryArchives = Archives<vide::PortableBinaryInputArchive, vide::PortableBinaryOutputArchive>;
using XMLArchives = Archives<vide::XMLInputArchive, vide::XMLOutputArchive>;
using JSONArchives = Archives<vide::JSONInputArchive, vide::JSONOutputArchive>;
using ProxyBinaryArchives = proxied<BinaryArchives, ProxyTestGroup>;
using ProxyPortableBinaryArchives = proxied<PortableBinaryArchives, ProxyTestGroup>;
using ProxyXMLArchives = proxied<XMLArchives, ProxyTestGroup>;
using ProxyJSONArchives = proxied<JSONArchives, ProxyTestGroup>;

TYPE_TO_STRING_AS("binary", BinaryArchives);
TYPE_TO_STRING_AS("portable_binary", PortableBinaryArchives);
TYPE_TO_STRING_AS("xml", XMLArchives);
TYPE_TO_STRING_AS("json", JSONArchives);
TYPE_TO_STRING_AS("proxy binary", ProxyBinaryArchives);
TYPE_TO_STRING_AS("proxy portable_binary", ProxyPortableBinaryArchives);
TYPE_TO_STRING_AS("proxy xml", ProxyXMLArchives);
TYPE_TO_STRING_AS("proxy json", ProxyJSONArchives);

// Type lists for TEST_CASE_TEMPLATE, one test case per entry:
// TEST_CASE_TEMPLATE("std::vector", Ar, VIDE_ALL_ARCHIVES) -> "std::vector<binary>", "std::vector<json>", ...
#define VIDE_BINARY_ARCHIVES BinaryArchives, PortableBinaryArchives
#define VIDE_TEXT_ARCHIVES XMLArchives, JSONArchives
#define VIDE_PROXY_BINARY_ARCHIVES ProxyBinaryArchives, ProxyPortableBinaryArchives
#define VIDE_PROXY_TEXT_ARCHIVES ProxyXMLArchives, ProxyJSONArchives
#define VIDE_ALL_BINARY_ARCHIVES VIDE_BINARY_ARCHIVES, VIDE_PROXY_BINARY_ARCHIVES
#define VIDE_ALL_TEXT_ARCHIVES VIDE_TEXT_ARCHIVES, VIDE_PROXY_TEXT_ARCHIVES
#define VIDE_ALL_ARCHIVES VIDE_ALL_BINARY_ARCHIVES, VIDE_ALL_TEXT_ARCHIVES
