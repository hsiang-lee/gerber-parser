find_package(gflags CONFIG REQUIRED)
find_package(blend2d CONFIG REQUIRED)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(gerber2image ${Source})
target_link_libraries(gerber2image PRIVATE gerber_engine gflags::gflags blend2d::blend2d)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})