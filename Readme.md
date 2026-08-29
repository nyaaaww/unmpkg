## A Tool for Decoding Wallpaper Engine's mpkg Files

### Overview
`unmpkg` is a program written in modern C++20 for decoding `.mpkg` files of
Wallpaper Engine. This program adheres to the principles of free software and
is released under the GNU General Public License (GPL).

### Features
- Command-line interface for extracting `.mpkg` archives.
- Recursively creates directories and extracts resources into the matching
  directory structure.
- Reports the archive version and the name/size of every contained file.
- Built-in robustness: malformed archives are detected and reported instead of
  crashing.
- The decoding logic lives in a reusable `unmpkg_core` library, ready for other
  tools to link against.

### Requirements
- A C++20 compiler (GCC 11+ or Clang 14+)
- CMake 3.16+
- A build tool such as Ninja or GNU Make

### Build it
```shell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The `unmpkg` executable will be produced in `build/`. To install it system-wide:

```shell
cmake --install build
```

### Run the tests
```shell
cmake -B build -G Ninja -DUNMPKG_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

### Usage
```text
Usage: unmpkg <filename> [options]

Options:
  -o, --output <dir>  Extract files below <dir> (default: current directory)
  -h, --help          Show this help message
```

Example:
```shell
unmpkg test.mpkg
unmpkg test.mpkg -o /tmp/extracted
```

### Project layout
```text
include/unmpkg/  Public headers (format definitions and the decoder API)
src/             Implementation and the command line entry point
tests/           Unit tests for the decoder
```

### Others
If you want to decode `.tex` files, see [repkg](https://github.com/notscuffed/repkg).
