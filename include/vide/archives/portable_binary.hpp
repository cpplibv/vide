/*! \file binary.hpp
    \brief Binary input and output archives */
/*
  Copyright (c) 2013-2022, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef VIDE_ARCHIVES_PORTABLE_BINARY_HPP_
#define VIDE_ARCHIVES_PORTABLE_BINARY_HPP_

#include <vide/vide.hpp>
#include <vide/concept.hpp>
#include <sstream>
#include <limits>


namespace vide {

// -------------------------------------------------------------------------------------------------

namespace portable_binary_detail {
//! Returns true if the current machine is little endian
/*! @ingroup Internal */
inline std::uint8_t is_little_endian() {
	static std::int32_t test = 1;
	return *reinterpret_cast<std::int8_t*>( &test ) == 1;
}

//! Swaps the order of bytes for some chunk of memory
/*! @param data The data as a uint8_t pointer
	@tparam DataSize The true size of the data
	@ingroup Internal */
template <std::size_t DataSize>
inline void swap_bytes(std::uint8_t* data) {
	for (std::size_t i = 0, end = DataSize / 2; i < end; ++i)
		std::swap(data[i], data[DataSize - i - 1]);
}
} // end namespace portable_binary_detail

// =================================================================================================

//! An output archive designed to save data in a compact binary representation portable over different architectures
/*! This archive outputs data to a stream in an extremely compact binary
	representation with as little extra metadata as possible.

	This archive will record the endianness of the data as well as the desired in/out endianness
	and assuming that the user takes care of ensuring serialized types are the same size
	across machines, is portable over different architectures.

	When using a binary archive and a file stream, you must use the
	std::ios::binary format flag to avoid having your data altered
	inadvertently.

	\warning This archive has not been thoroughly tested across different architectures.
			 Please report any issues, optimizations, or feature requests at
			 <a href="www.github.com/USCiLab/cereal">the project github</a>.

  \ingroup Archives */
class PortableBinaryOutputArchive :
		public OutputArchive<PortableBinaryOutputArchive, AllowEmptyClassElision | IgnoreNVP | BinaryArchive> {

private:
	std::ostream& itsStream;
	const uint8_t itsConvertEndianness; //!< If set to true, we will need to swap bytes upon saving

public:
	//! A class containing various advanced options for the PortableBinaryOutput archive
	class Options {
	public:
		//! Represents desired endianness
		enum class Endianness : std::uint8_t { big, little };

		//! Default options, preserve system endianness
		static Options Default() { return Options(); }

		//! Save as little endian
		static Options LittleEndian() { return Options(Endianness::little); }

		//! Save as big endian
		static Options BigEndian() { return Options(Endianness::big); }

		//! Specify specific options for the PortableBinaryOutputArchive
		/*! @param outputEndian The desired endianness of saved (output) data */
		explicit Options(Endianness outputEndian = getEndianness()) :
				itsOutputEndianness(outputEndian) {}

	private:
		//! Gets the endianness of the system
		inline static Endianness getEndianness() { return portable_binary_detail::is_little_endian() ? Endianness::little : Endianness::big; }

		//! Checks if Options is set for little endian
		inline std::uint8_t is_little_endian() const { return itsOutputEndianness == Endianness::little; }

		friend class PortableBinaryOutputArchive;

		Endianness itsOutputEndianness;
	};

	//! Construct, outputting to the provided stream
	/*! @param stream The stream to output to. Should be opened with std::ios::binary flag.
		@param options The PortableBinary specific options to use.  See the Options struct
					   for the values of default parameters */
	explicit PortableBinaryOutputArchive(std::ostream& stream, Options const& options = Options::Default()) :
			itsStream(stream),
			itsConvertEndianness(portable_binary_detail::is_little_endian() ^ options.is_little_endian()) {
		this->operator()(options.is_little_endian());
	}

	~PortableBinaryOutputArchive() noexcept = default;

	//! Writes size bytes of data to the output stream
	template <std::streamsize DataSize>
	inline void saveBinary(const void* data, std::streamsize size) {
		std::streamsize writtenSize = 0;

		if (itsConvertEndianness) {
			for (std::streamsize i = 0; i < size; i += DataSize)
				for (std::streamsize j = 0; j < DataSize; ++j)
					writtenSize += itsStream.rdbuf()->sputn(reinterpret_cast<const char*>( data ) + DataSize - j - 1 + i, 1);
		} else
			writtenSize = itsStream.rdbuf()->sputn(reinterpret_cast<const char*>( data ), size);

		if (writtenSize != size)
			throw Exception("Failed to write " + std::to_string(size) + " bytes to output stream. Wrote " + std::to_string(writtenSize));
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
		static_assert(!std::is_floating_point_v<T> || std::numeric_limits<T>::is_iec559,
				"Portable binary only supports IEEE 754 standardized floating point");

		saveBinary<sizeof(T)>(std::addressof(t), sizeof(t));
	}

	template <class As, class T>
	inline void process_as(As&, const BinaryData<T>& t) {
		using TT = std::remove_pointer_t<T>;

		static_assert(!std::is_floating_point_v<TT> || std::numeric_limits<TT>::is_iec559,
				"Portable binary only supports IEEE 754 standardized floating point");

		saveBinary<sizeof(TT)>(t.data, static_cast<std::streamsize>(t.size));
	}
};

// =================================================================================================

//! An input archive designed to load data saved using PortableBinaryOutputArchive
/*! This archive outputs data to a stream in an extremely compact binary
	representation with as little extra metadata as possible.

	This archive will load the endianness of the serialized data and
	if necessary transform it to match that of the local machine.  This comes
	at a significant performance cost compared to non portable archives if
	the transformation is necessary, and also causes a small performance hit
	even if it is not necessary.

	It is recommended to use portable archives only if you know that you will
	be sending binary data to machines with different endianness.

	The archive will do nothing to ensure types are the same size - that is
	the responsibility of the user.

	When using a binary archive and a file stream, you must use the
	std::ios::binary format flag to avoid having your data altered
	inadvertently.

	\warning This archive has not been thoroughly tested across different architectures.
			 Please report any issues, optimizations, or feature requests at
			 <a href="www.github.com/USCiLab/cereal">the project github</a>.

  \ingroup Archives */
class PortableBinaryInputArchive : public InputArchive<PortableBinaryInputArchive, AllowEmptyClassElision | IgnoreNVP | BinaryArchive> {
private:
	std::istream::pos_type end;
	std::istream& itsStream;
	uint8_t itsConvertEndianness; //!< If set to true, we will need to swap bytes upon loading

public:
	//! A class containing various advanced options for the PortableBinaryInput archive
	class Options {
	public:
		//! Represents desired endianness
		enum class Endianness : std::uint8_t { big, little };

		//! Default options, preserve system endianness
		static Options Default() { return Options(); }

		//! Load into little endian
		static Options LittleEndian() { return Options(Endianness::little); }

		//! Load into big endian
		static Options BigEndian() { return Options(Endianness::big); }

		//! Specify specific options for the PortableBinaryInputArchive
		/*! @param inputEndian The desired endianness of loaded (input) data */
		explicit Options(Endianness inputEndian = getEndianness()) :
				itsInputEndianness(inputEndian) {}

	private:
		//! Gets the endianness of the system
		inline static Endianness getEndianness() { return portable_binary_detail::is_little_endian() ? Endianness::little : Endianness::big; }

		//! Checks if Options is set for little endian
		inline std::uint8_t is_little_endian() const { return itsInputEndianness == Endianness::little; }

		friend class PortableBinaryInputArchive;

		Endianness itsInputEndianness;
	};

	//! Construct, loading from the provided stream
	/*! @param stream The stream to read from. Should be opened with std::ios::binary flag.
		@param options The PortableBinary specific options to use.  See the Options struct
					   for the values of default parameters */
	explicit PortableBinaryInputArchive(std::istream& stream, Options const& options = Options::Default()) :
			itsStream(stream),
			itsConvertEndianness(false) {

		const auto begin = itsStream.tellg();
		itsStream.seekg(0, std::ios::end);
		if (itsStream.fail())
			throw Exception("Failed to seek stream to determine binary size.");
		end = itsStream.tellg();
		itsStream.seekg(begin, std::ios::beg);
		if (itsStream.fail())
			throw Exception("Failed to seek stream to determine binary size.");
		reserveMemoryBudget = static_cast<std::size_t>(end - begin) * VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER;

		uint8_t streamLittleEndian;
		this->operator()(streamLittleEndian);
		itsConvertEndianness = options.is_little_endian() ^ streamLittleEndian;
	}

	~PortableBinaryInputArchive() noexcept = default;

	//! Reads size bytes of data from the input stream
	/*! @param data The data to save
		@param size The number of bytes in the data
		@tparam DataSize T The size of the actual type of the data elements being loaded */
	template <std::streamsize DataSize>
	inline void loadBinary(void* const data, std::streamsize size) {
		// load data
		auto const readSize = itsStream.rdbuf()->sgetn(reinterpret_cast<char*>( data ), size);

		if (readSize != size)
			throw Exception("Failed to read " + std::to_string(size) + " bytes from input stream. Read " + std::to_string(readSize));

		// flip bits if needed
		if (itsConvertEndianness) {
			std::uint8_t* ptr = reinterpret_cast<std::uint8_t*>( data );
			for (std::streamsize i = 0; i < size; i += DataSize)
				portable_binary_detail::swap_bytes<DataSize>(ptr + i);
		}
	}

	[[nodiscard]] inline std::size_t maximumBinaryReadSize() const {
		const auto pos = itsStream.tellg();
		return end - pos;
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
		static_assert(!std::is_floating_point_v<T> || std::numeric_limits<T>::is_iec559,
				"Portable binary only supports IEEE 754 standardized floating point");

		loadBinary<sizeof(T)>(std::addressof(t), sizeof(t));
	}

	template <class As, class T>
	inline void process_as(As&, BinaryData<T>& t) {
		using TT = std::remove_pointer_t<T>;

		static_assert(!std::is_floating_point_v<TT> || std::numeric_limits<TT>::is_iec559,
				"Portable binary only supports IEEE 754 standardized floating point");

		loadBinary<sizeof(TT)>(t.data, static_cast<std::streamsize>(t.size));
	}
};

// =================================================================================================

} // namespace vide

// register archives for polymorphic support
VIDE_REGISTER_ARCHIVE(vide::PortableBinaryOutputArchive)
VIDE_REGISTER_ARCHIVE(vide::PortableBinaryInputArchive)

#endif // VIDE_ARCHIVES_PORTABLE_BINARY_HPP_
