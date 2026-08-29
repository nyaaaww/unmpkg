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

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace
{

constexpr std::string_view kUsage = "A tool to decode Wallpaper Engine's mpkg files.\n"
				    "\n"
				    "Usage: unmpkg <filename> [options]\n"
				    "\n"
				    "Options:\n"
				    "  -o, --output <dir>  Extract files below <dir> (default: current directory)\n"
				    "  -h, --help          Show this help message\n";

struct Options {
	std::string input_file;
	std::filesystem::path output_dir;
};

void print_header(const unmpkg::MpkgHeader &header)
{
	std::cout << "Version length: " << header.version_length << '\n';
	std::cout << "Version: " << header.version_string() << '\n';
	std::cout << "Number of files: " << header.file_total << '\n';
}

void print_entries(const std::vector<unmpkg::MpkgFileEntry> &entries)
{
	for (const auto &entry : entries) {
		std::cout << "\nName length: " << entry.file_name_length << '\n';
		std::cout << "Name: " << entry.file_name << '\n';
		std::cout << "File size: " << entry.file_size << '\n';
	}
}

std::optional<Options> parse_args(int argc, char *argv[])
{
	Options options;
	for (int i = 1; i < argc; ++i) {
		const std::string_view arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			std::cout << kUsage;
			return std::nullopt;
		}
		if (arg == "-o" || arg == "--output") {
			if (i + 1 >= argc) {
				std::cerr << "unmpkg: missing argument for '" << arg << "'\n";
				return std::nullopt;
			}
			options.output_dir = argv[++i];
		} else if (!arg.empty() && arg.front() == '-') {
			std::cerr << "unmpkg: unknown option '" << arg << "'\n";
			return std::nullopt;
		} else if (options.input_file.empty()) {
			options.input_file = std::string(arg);
		} else {
			std::cerr << "unmpkg: too many arguments\n";
			return std::nullopt;
		}
	}

	if (options.input_file.empty()) {
		std::cerr << kUsage;
		return std::nullopt;
	}
	return options;
}

int run(int argc, char *argv[])
{
	const auto options = parse_args(argc, argv);
	if (!options) {
		return EXIT_FAILURE;
	}

	std::ifstream input(options->input_file, std::ios::binary);
	if (!input) {
		std::cerr << "unmpkg: cannot open '" << options->input_file << "'\n";
		return EXIT_FAILURE;
	}

	try {
		unmpkg::MpkgDecoder decoder(input);
		print_header(decoder.header());
		print_entries(decoder.entries());
		std::cout << "\nExtracting files...\n";
		decoder.extract_all(options->output_dir);
	} catch (const unmpkg::MpkgError &error) {
		std::cerr << "unmpkg: " << error.what() << '\n';
		return EXIT_FAILURE;
	} catch (const std::exception &error) {
		std::cerr << "unmpkg: unexpected error: " << error.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

} // namespace

int main(int argc, char *argv[])
{
	return run(argc, argv);
}
