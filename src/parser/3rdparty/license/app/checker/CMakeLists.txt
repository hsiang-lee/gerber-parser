file(GLOB_RECURSE srcs *.cpp)

add_executable(license_check ${srcs})

target_include_directories(license_check PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

target_link_libraries(license_check
    license
)
