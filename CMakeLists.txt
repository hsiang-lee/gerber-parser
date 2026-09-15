cmake_minimum_required(VERSION 3.20)

project(gerber-parser C CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS OFF)
option(BUILD_EXAMPLES OFF)

add_subdirectory(src)

if(BUILD_EXAMPLES)
	add_subdirectory(example/gerber2image)
endif()

if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    file(GLOB_RECURSE AllCppFile ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    add_custom_target(
        Tidy
        clang-tidy
        ${AllCppFile}
        -format-style=file
        -checks=-*,cppcoreguidelines-*,modernize-*,performance-*,portability-*,readability-*,concurrency-*,clang-analyzer-*,bugprone-*,-modernize-use-trailing-return-type
		-export-fixes="${PROJECT_BINARY_DIR}/clang-tidy-fixes.yml"
        --
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

endif()