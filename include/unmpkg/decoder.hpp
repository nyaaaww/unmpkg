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

#include "unmpkg/mpkg.hpp"

#include <filesystem>
#include <istream>
#include <stdexcept>
#include <vector>

namespace unmpkg
{

/// Thrown whenever a `.mpkg` stream is malformed or cannot be written out.
class MpkgError : public std::runtime_error {
    public:
	using std::runtime_error::runtime_error;
};

/// Decodes a Wallpaper Engine `.mpkg` stream and extracts its contents.
///
/// The decoder reads the file header and the directory entries when it is
/// constructed, leaving the input stream positioned at the first payload
/// block. Extraction copies each payload out of the stream into the file
/// system.
class MpkgDecoder {
    public:
	explicit MpkgDecoder(std::istream &input);
	MpkgDecoder(const MpkgDecoder &) = delete;
	MpkgDecoder &operator=(const MpkgDecoder &) = delete;

	const MpkgHeader &header() const noexcept
	{
		return header_;
	}
	const std::vector<MpkgFileEntry> &entries() const noexcept
	{
		return entries_;
	}

	/// Writes every entry's payload below `output_dir`.
	void extract_all(const std::filesystem::path &output_dir = std::filesystem::path());

    private:
	std::uint32_t read_u32(const char *what);
	void read_fixed_bytes(char *dest, std::size_t length, const char *what);
	std::string read_string(std::uint32_t length, const char *what);
	void read_header();
	void read_entries();
	void extract(const MpkgFileEntry &entry, const std::filesystem::path &base);

	std::istream &input_;
	MpkgHeader header_;
	std::vector<MpkgFileEntry> entries_;
};

} // namespace unmpkg
