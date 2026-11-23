//

#pragma once

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>


/// Debugging aid for binary archive output: 32 bytes per line, hex then printable ASCII
[[nodiscard]] inline std::string hex_dump_with_ascii(std::string_view s) {
	if (s.empty())
		return "";

	std::ostringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase;

	std::size_t i = 0;
	while (true) {
		ss << std::setw(2) << std::uint16_t{static_cast<unsigned char>(s[i])};
		++i;

		if (i == s.size())
			break;

		if (i % 32 == 0) {
			ss << " | ";
			for (std::size_t j = i - 32; j < i;) {
				ss << (std::isprint(static_cast<unsigned char>(s[j])) ? s[j] : '.');
				++j;
				if (j % 8 == 0 && j % 32 != 0)
					ss << ' ';
			}
			ss << "\n";
		} else if (i % 8 == 0) {
			ss << "  ";
		} else {
			ss << ' ';
		}
	}

	if (i % 32 != 0) {
		const auto r_begin = i;
		const auto r_end = (i / 32 + 1) * 32;

		for (std::size_t r = r_begin; r < r_end; ++r) {
			if (r % 32 == 0) {
				ss << " ";
			} else if (r % 8 == 0) {
				ss << "    ";
			} else {
				ss << "   ";
			}
		}
	}

	ss << " | ";
	for (std::size_t j = (i - 1) / 32 * 32; j < i;) {
		ss << (std::isprint(static_cast<unsigned char>(s[j])) ? s[j] : '.');
		++j;
		if (j % 8 == 0 && j % 32 != 0)
			ss << ' ';
	}

	return std::move(ss).str();
}
