#pragma once


namespace vide { // --------------------------------------------------------------------------------

/// Tag of a value serialized out of place, created by ar.defer(value): it is written or loaded at the
/// next ar.serializeDeferments(). Keeps recursion shallow through long pointer chains.
/// T is a reference for an lvalue; a saved rvalue is kept as a copy until then
template <class T>
struct DeferredData {
	T value;
};

} // namespace vide --------------------------------------------------------------------------------
