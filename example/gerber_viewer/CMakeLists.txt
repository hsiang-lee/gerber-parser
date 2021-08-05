find_package(Qt5 COMPONENTS OpenGL REQUIRED)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(gerber_viewer ${Source})
target_link_libraries(gerber_viewer PRIVATE gerber_render Qt5::OpenGL)
target_link_options(gerber_viewer PRIVATE "/ENTRY:mainCRTStartup" "/SUBSYSTEM:WINDOWS")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})
