#pragma once

#include <vide/vide.hpp>
#include <vide/concept.hpp>
#include <sstream>


namespace vide {

// =================================================================================================

//! An output archive designed to save data in a compact binary representation
/*! This archive outputs data to a stream in an extremely compact binary
	representation with as little extra metadata as possible.

	This archive does nothing to ensure that the endianness of the saved
	and loaded data is the same.  If you need to have portability over
	architectures with different endianness, use PortableBinaryOutputArchive.

	When using a binary archive and a file stream, you must use the
	std::ios::binary format flag to avoid having your data altered
	inadvertently.

	\ingroup Archives */
class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive, AllowEmptyClassElision | IgnoreNVP> {
private:
	std::ostream& itsStream;

public:
	//! Construct, outputting to the provided stream
	/*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
					  even cout! */
	explicit BinaryOutputArchive(std::ostream& stream) :
		itsStream(stream) {
	}

	~BinaryOutputArchive() noexcept = default;

	//! Writes size bytes of data to the output stream
	void saveBinary(const void* data, std::streamsize size) {
		auto const writtenSize = itsStream.rdbuf()->sputn(reinterpret_cast<const char*>(data), size);

		if (writtenSize != size)
			throw Exception("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " + std::to_string(writtenSize));
	}

	// --- process_as remapping ------------------------------------------------------------------------
public:
	using OutputArchive::process_as;

	template <class As, typename T>
	inline void process_as(As& as, const NameValuePair<T>& t) {
		as(t.value);
	}

	template <class As, typename T>
	inline void process_as(As& as, const SizeTag<T>& t) {
		as(t.size);
	}

	template <class As, arithmetic T>
	inline void process_as(As&, const T& t) {
		saveBinary(std::addressof(t), sizeof(t));
	}

	template <class As, class T>
	inline void process_as(As&, const BinaryData<T>& t) {
		saveBinary(t.data, static_cast<std::streamsize>(t.size));
	}
};

// =================================================================================================

//! An input archive designed to load data saved using BinaryOutputArchive
/*  This archive does nothing to ensure that the endianness of the saved
	and loaded data is the same.  If you need to have portability over
	architectures with different endianness, use PortableBinaryOutputArchive.

	When using a binary archive and a file stream, you must use the
	std::ios::binary format flag to avoid having your data altered
	inadvertently.

	\ingroup Archives */
class BinaryInputArchive : public InputArchive<BinaryInputArchive, AllowEmptyClassElision | IgnoreNVP> {
private:
	std::istream& itsStream;

public:
	//! Construct, loading from the provided stream
	explicit BinaryInputArchive(std::istream& stream) :
		itsStream(stream) {
	}

	~BinaryInputArchive() noexcept = default;

	//! Reads size bytes of data from the input stream
	void loadBinary(void* data, std::streamsize size) {
		const auto readSize = itsStream.rdbuf()->sgetn(reinterpret_cast<char*>(data), size);

		if (readSize != size)
			throw Exception("Failed to read " + std::to_string(size) + " bytes from input stream! Read " + std::to_string(readSize));
	}

	// --- process_as remapping ------------------------------------------------------------------------
public:
	using InputArchive::process_as;

	template <class As, typename T>
	inline void process_as(As& as, NameValuePair<T>& t) {
		as(t.value);
	}

	template <class As, typename T>
	inline void process_as(As& as, SizeTag<T>& t) {
		as(t.size);
	}

	template <class As, arithmetic T>
	inline void process_as(As&, T& t) {
		loadBinary(std::addressof(t), sizeof(t));
	}

	template <class As, class T>
	inline void process_as(As&, BinaryData<T>& t) {
		loadBinary(t.data, static_cast<std::streamsize>(t.size));
	}
};

// =================================================================================================

} // namespace vide

// register archives for polymorphic support
VIDE_REGISTER_ARCHIVE(vide::BinaryOutputArchive)
VIDE_REGISTER_ARCHIVE(vide::BinaryInputArchive)
