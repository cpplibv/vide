//

#pragma once

#include <vide/archives/proxy_archive.hpp>


/// Minimal user proxy; carries user state to show that proxies may extend the archive
template <typename Ar>
struct UserProxyArchive : vide::ProxyArchive<UserProxyArchive<Ar>, Ar> {
	int my_user_data = 42;

	using vide::ProxyArchive<UserProxyArchive<Ar>, Ar>::ProxyArchive;
};

/// Base holding the wrapped archive, so it is constructed before the proxy that refers to it
template <typename Ar>
struct ProxyStorage {
	Ar ar;

	template <typename Stream>
	explicit ProxyStorage(Stream& stream) :
		ar(stream) {}
};

/// Owns an archive and exposes it through UserProxyArchive; constructible from a stream like Ar itself
template <typename Ar>
struct ProxyTestGroup : ProxyStorage<Ar>, UserProxyArchive<Ar> {
	template <typename Stream>
	explicit ProxyTestGroup(Stream& stream) :
		ProxyStorage<Ar>(stream),
		UserProxyArchive<Ar>(ProxyStorage<Ar>::ar) {}
};
