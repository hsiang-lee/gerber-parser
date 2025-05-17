file(GLOB_RECURSE srcs *.cpp)

add_executable(key_gen ${srcs})

target_include_directories(key_gen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

target_link_libraries(key_gen
    license
)
