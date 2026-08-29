add_executable(unmpkg_tests decoder_test.cpp)
target_link_libraries(unmpkg_tests PRIVATE unmpkg::core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(unmpkg_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME unmpkg_tests COMMAND unmpkg_tests)
