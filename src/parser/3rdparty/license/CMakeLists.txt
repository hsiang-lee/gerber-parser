cmake_minimum_required(VERSION 3.20)

option(BUILD_GENERATOR "whether to build license generator" ON)
option(BUILD_TESTS "whether to build tests" OFF)

if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()

project(license C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT TARGET nlohmann_json)
    add_subdirectory(thirdparty/json)
endif()

add_subdirectory(thirdparty/libsodium)
add_subdirectory(src)

if (BUILD_GENERATOR)
    add_subdirectory(app)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif() 
