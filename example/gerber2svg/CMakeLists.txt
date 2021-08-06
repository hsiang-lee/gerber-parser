find_package(Qt5 COMPONENTS Svg REQUIRED)
set(CMAKE_AUTORCC ON)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(gerber2svg ${Source} ${Resource})
target_link_libraries(gerber2svg PRIVATE gerber_render gflags Qt5::Svg)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})
