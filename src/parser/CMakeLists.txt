cmake_minimum_required(VERSION 3.20)

project(gerber_parser LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")

if (NOT APPLE)
	set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE}   -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility=hidden")
	set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE}   -fvisibility-inlines-hidden")
	set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility-inlines-hidden")

	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
	set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--gc-sections")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
endif()

if (WIN32)
	add_definitions( /Zi )
	add_definitions( /Od )
endif()

set(BUILD_GENERATOR OFF)
set(JSON_BuildTests OFF)
set(JSON_Install OFF)
set(JSON_ImplicitConversions OFF)
option(BUILD_TESTS OFF)
add_subdirectory(3rdparty/license)
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(3rdparty/googletest)

	enable_testing()
	add_subdirectory(tests)
endif()

add_subdirectory(example)

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
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src/gerber"
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src/engine"
		-I "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

endif()
