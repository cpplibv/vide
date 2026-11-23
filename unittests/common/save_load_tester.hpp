//

#pragma once

#include <sstream>


/// Round trips a value and reports whether it came back equal. Validators only apply on load: the value is saved
/// unchecked, so invalid data reaches the input side
template <typename IArchive, typename OArchive>
struct SaveLoadTester {
	template <typename T>
	bool operator()(const T& output, const auto&... validators) {
		{ // Saving accepts the validators too
			std::ostringstream os;
			OArchive oar(os);
			oar(output, validators...);
		}

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(output);
		}

		T input{};
		std::istringstream is(os.str());
		{
			IArchive iar(is);
			iar(input, validators...);
		}

		return output == input;
	}
};
