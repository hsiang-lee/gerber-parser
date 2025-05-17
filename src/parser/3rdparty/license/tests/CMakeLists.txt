file(GLOB_RECURSE srcs *.cpp)

add_executable(tests ${srcs})

target_include_directories(tests PUBLIC
    ${Json_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(tests
    license
    gtest
    gtest_main
)
