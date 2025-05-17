file(GLOB_RECURSE srcs *.cpp)

add_executable(machine_id ${srcs})

target_include_directories(machine_id PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

target_link_libraries(machine_id
    license
)
