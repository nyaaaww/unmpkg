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

#include "unmpkg/decoder.hpp"

#include <algorithm>
#include <array>
#include <fstream>

namespace fs = std::filesystem;

namespace unmpkg
{

std::string MpkgHeader::version_string() const
{
	return std::string(version.data(), version_length);
}

namespace
{

[[noreturn]] void fail(const std::string &message)
{
	throw MpkgError(message);
}

} // namespace

MpkgDecoder::MpkgDecoder(std::istream &input)
	: input_(input)
{
	read_header();
	read_entries();
}

std::uint32_t MpkgDecoder::read_u32(const char *what)
{
	std::uint32_t value = 0;
	input_.read(reinterpret_cast<char *>(&value), sizeof(value));
	if (!input_) {
		fail(std::string("unexpected end of file while reading ") + what);
	}
	return value;
}

void MpkgDecoder::read_fixed_bytes(char *dest, std::size_t length, const char *what)
{
	if (length > 0) {
		input_.read(dest, static_cast<std::streamsize>(length));
		if (!input_) {
			fail(std::string("unexpected end of file while reading ") + what);
		}
	}
}

std::string MpkgDecoder::read_string(std::uint32_t length, const char *what)
{
	std::string result(length, '\0');
	read_fixed_bytes(result.data(), result.size(), what);
	return result;
}

void MpkgDecoder::read_header()
{
	header_.version_length = read_u32("version length");
	if (header_.version_length > kVersionFieldSize) {
		fail("corrupt mpkg: version length exceeds field size");
	}
	read_fixed_bytes(header_.version.data(), kVersionFieldSize, "version");
	header_.file_total = read_u32("file count");
}

void MpkgDecoder::read_entries()
{
	entries_.reserve(header_.file_total);
	for (std::uint32_t i = 0; i < header_.file_total; ++i) {
		MpkgFileEntry entry;
		entry.file_name_length = read_u32("file name length");
		if (entry.file_name_length > kMaxFileNameLength) {
			fail("corrupt mpkg: file name length exceeds limit");
		}
		entry.file_name = read_string(entry.file_name_length, "file name");
		entry.index = read_u32("file index");
		entry.file_size = read_u32("file size");
		entries_.push_back(std::move(entry));
	}
}

void MpkgDecoder::extract_all(const fs::path &output_dir)
{
	for (const auto &entry : entries_) {
		extract(entry, output_dir);
	}
}

void MpkgDecoder::extract(const MpkgFileEntry &entry, const fs::path &base)
{
	const fs::path destination = base / entry.file_name;

	if (destination.has_parent_path()) {
		std::error_code ec;
		fs::create_directories(destination.parent_path(), ec);
		if (ec) {
			fail("cannot create directory '" + destination.parent_path().string() + "': " + ec.message());
		}
	}

	std::ofstream output(destination, std::ios::binary | std::ios::trunc);
	if (!output) {
		fail("cannot open '" + destination.string() + "' for writing");
	}

	std::array<char, 65536> buffer{};
	std::size_t remaining = entry.file_size;
	while (remaining > 0) {
		const std::size_t chunk = std::min<std::size_t>(remaining, buffer.size());
		read_fixed_bytes(buffer.data(), chunk, "file data");
		output.write(buffer.data(), static_cast<std::streamsize>(chunk));
		remaining -= chunk;
	}

	if (!output) {
		fail("failed writing '" + destination.string() + "'");
	}
}

} // namespace unmpkg
