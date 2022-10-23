cmake_minimum_required(VERSION 3.20)

project(gerber-parser C CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS OFF)
option(BUILD_EXAMPLES OFF)

if (WIN32)
	message(STATUS "Now is windows")
	set(gerber-parser_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/cmake)
elseif (APPLE)
	message(STATUS "Now is Apple systens.")
	set(gerber-parser_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/macos/cmake)
elseif (UNIX)
	message(STATUS "Now is UNIX-like OS's.")
	set(gerber-parser_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/ubuntu/cmake)
endif ()
find_package(gerber-parser REQUIRED)

if(BUILD_EXAMPLES)
	add_subdirectory(3rdparty/gflags)
	
	add_subdirectory(example/gerber2image)
	add_subdirectory(example/gerber_viewer)
endif()

if(BUILD_TESTS)
	add_subdirectory(3rdparty/googletest)

	enable_testing()
	add_subdirectory(tests)
endif()

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
	find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
    file(GLOB_RECURSE AllCppFile ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    add_custom_target(
        Tidy
        clang-tidy
        ${AllCppFile}
        -format-style=file
        -checks=-*,cppcoreguidelines-*,modernize-*,performance-*,portability-*,readability-*,concurrency-*,clang-analyzer-*,bugprone-*,-modernize-use-trailing-return-type
		-export-fixes="${PROJECT_BINARY_DIR}/clang-tidy-fixes.yml"
        --
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src/gerber"
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src/engine"
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src"
		-I "${Qt5Widgets_INCLUDE_DIRS}"
    )

endif()
