file(GLOB_RECURSE srcs *.cpp)

add_executable(license_gen ${srcs})

target_include_directories(license_gen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

target_link_libraries(license_gen
    license
)
