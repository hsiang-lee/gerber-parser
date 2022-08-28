cmake_minimum_required(VERSION 3.20)

project(gerber_parser C CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS OFF)
option(BUILD_EXAMPLES OFF)

add_subdirectory(3rdparty/glog)
target_compile_definitions(glog PRIVATE "HAVE_SNPRINTF")
add_subdirectory(src)

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
		-I "${PROJECT_BINARY_DIR}/3rdparty/glog"
		-I "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glog/src"
		-I "${Qt5Widgets_INCLUDE_DIRS}"
    )

endif()
