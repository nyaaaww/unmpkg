cmake_minimum_required(VERSION 3.16)

project(
	unmpkg
	VERSION 1.0.0
	DESCRIPTION "A tool for decoding Wallpaper Engine's mpkg files"
	HOMEPAGE_URL "https://github.com/393d2d48-2386/unmpkg"
	LANGUAGES CXX)

include(GNUInstallDirs)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

option(UNMPKG_BUILD_TESTS "Build the unmpkg tests" ${BUILD_TESTING})
option(UNMPKG_INSTALL "Generate install and export rules" ON)

set(UNMPKG_WARNINGS -Wall -Wextra -Wpedantic)

# ---------------------------------------------------------------------------
# Core library: the mpkg format decoder. Reusable by other tools.
# ---------------------------------------------------------------------------
add_library(
	unmpkg_core
	src/decoder.cpp
	include/unmpkg/mpkg.hpp
	include/unmpkg/decoder.hpp)
add_library(unmpkg::core ALIAS unmpkg_core)

target_include_directories(
	unmpkg_core
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(unmpkg_core PUBLIC cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(unmpkg_core PRIVATE ${UNMPKG_WARNINGS})
endif()

# ---------------------------------------------------------------------------
# Command line tool
# ---------------------------------------------------------------------------
add_executable(unmpkg src/main.cpp)
target_link_libraries(unmpkg PRIVATE unmpkg::core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(unmpkg PRIVATE ${UNMPKG_WARNINGS})
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(UNMPKG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------
if(UNMPKG_INSTALL)
	install(
		TARGETS unmpkg unmpkg_core
		EXPORT unmpkgTargets
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(
		EXPORT unmpkgTargets
		FILE unmpkgConfig.cmake
		NAMESPACE unmpkg::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/unmpkg)
endif()
