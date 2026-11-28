#pragma once

#include <cstddef>


namespace vide { // --------------------------------------------------------------------------------

/// Tag of a contiguous run of elements, created by ar.binary_data(data, size). Binary archives write it
/// as one block of raw memory; portable binary swaps the bytes of each element on its own.
/// Typed by element size only, not by element type: one process_as per size instead of one per type
template <std::size_t ElementSize>
struct BinaryData {
	/// Non-const even when saving: output archives only read through it
	void* data;
	/// Number of elements, not bytes
	std::size_t size;
};

} // namespace vide --------------------------------------------------------------------------------
