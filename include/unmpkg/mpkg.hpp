/*
 * Copyright (C) 2024 nyaaaww
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace unmpkg
{

/// Maximum length of an on-disk file name, also the size of the fixed field.
constexpr std::uint32_t kMaxFileNameLength = 255;

/// Size of the fixed `version` field in the header.
constexpr std::size_t kVersionFieldSize = 8;

/// File format definition for Wallpaper Engine's `.mpkg` container.
/// All multi-byte fields are stored in little-endian order.
struct MpkgHeader {
	std::uint32_t version_length = 0;
	std::array<char, kVersionFieldSize> version{};
	std::uint32_t file_total = 0;

	/// Returns the version field trimmed to `version_length` bytes.
	std::string version_string() const;
};

struct MpkgFileEntry {
	std::uint32_t file_name_length = 0;
	std::string file_name;
	std::uint32_t index = 0;
	std::uint32_t file_size = 0;
};

} // namespace unmpkg
