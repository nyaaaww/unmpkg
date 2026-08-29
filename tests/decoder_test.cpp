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
 * along with this program.  If not, see <see http://www.gnu.org/licenses/>.
 */

#include "unmpkg/decoder.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace
{

void require(bool condition, const std::string &message)
{
	if (!condition) {
		std::cerr << "FAILED: " << message << '\n';
		std::exit(EXIT_FAILURE);
	}
}

void write_u32(std::ostringstream &out, std::uint32_t value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(value));
}

void write_bytes(std::ostringstream &out, const char *data, std::size_t size)
{
	out.write(data, static_cast<std::streamsize>(size));
}

void build_mpkg(std::ostringstream &out)
{
	write_u32(out, 2);
	write_bytes(out, "v1\0\0\0\0\0\0", unmpkg::kVersionFieldSize);
	write_u32(out, 2);

	const char *name1 = "hello.txt";
	write_u32(out, static_cast<std::uint32_t>(std::strlen(name1)));
	write_bytes(out, name1, std::strlen(name1));
	write_u32(out, 0);
	write_u32(out, 5);

	const char *name2 = "sub/foo.txt";
	write_u32(out, static_cast<std::uint32_t>(std::strlen(name2)));
	write_bytes(out, name2, std::strlen(name2));
	write_u32(out, 1);
	write_u32(out, 3);

	write_bytes(out, "hello", 5);
	write_bytes(out, "bar", 3);
}

void test_parse()
{
	std::ostringstream raw;
	build_mpkg(raw);
	std::istringstream input(raw.str());
	unmpkg::MpkgDecoder decoder(input);

	require(decoder.header().version_length == 2, "version length");
	require(decoder.header().version_string() == "v1", "version string");
	require(decoder.header().file_total == 2, "file total");

	const auto &entries = decoder.entries();
	require(entries.size() == 2, "entry count");
	require(entries[0].file_name == "hello.txt", "first entry name");
	require(entries[0].file_size == 5, "first entry size");
	require(entries[1].file_name == "sub/foo.txt", "second entry name");
	require(entries[1].file_size == 3, "second entry size");
}

void test_extract()
{
	std::ostringstream raw;
	build_mpkg(raw);
	std::istringstream input(raw.str());
	unmpkg::MpkgDecoder decoder(input);

	const auto out_dir = std::filesystem::temp_directory_path() / "unmpkg_test";
	std::filesystem::remove_all(out_dir);
	decoder.extract_all(out_dir);

	std::ifstream f1(out_dir / "hello.txt", std::ios::binary);
	require(static_cast<bool>(f1), "hello.txt created");
	std::string content1{ std::istreambuf_iterator<char>(f1), std::istreambuf_iterator<char>() };
	require(content1 == "hello", "hello.txt content");

	std::ifstream f2(out_dir / "sub/foo.txt", std::ios::binary);
	require(static_cast<bool>(f2), "sub/foo.txt created");
	std::string content2{ std::istreambuf_iterator<char>(f2), std::istreambuf_iterator<char>() };
	require(content2 == "bar", "sub/foo.txt content");

	std::filesystem::remove_all(out_dir);
}

void test_corrupt_input()
{
	{
		std::istringstream input("x");
		bool threw = false;
		try {
			unmpkg::MpkgDecoder decoder(input);
		} catch (const unmpkg::MpkgError &) {
			threw = true;
		}
		require(threw, "truncated header must throw");
	}

	{
		std::ostringstream raw;
		write_u32(raw, 255);
		write_bytes(raw, "v1\0\0\0\0\0\0", unmpkg::kVersionFieldSize);
		write_u32(raw, 1);
		write_u32(raw, 300); // name length exceeds kMaxFileNameLength
		bool threw = false;
		try {
			std::istringstream input(raw.str());
			unmpkg::MpkgDecoder decoder(input);
		} catch (const unmpkg::MpkgError &) {
			threw = true;
		}
		require(threw, "oversized name length must throw");
	}
}

} // namespace

int main()
{
	test_parse();
	test_extract();
	test_corrupt_input();
	std::cout << "all tests passed\n";
	return EXIT_SUCCESS;
}
