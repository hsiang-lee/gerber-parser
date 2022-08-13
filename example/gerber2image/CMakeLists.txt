find_package(Qt5 COMPONENTS Core)
set(CMAKE_AUTORCC ON)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB Resource ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

add_executable(gerber2image ${Source} ${Resource} "app_win32.rc")
target_link_libraries(gerber2image PRIVATE gerber_renderer gflags)
#target_link_options(gerber2image PRIVATE "/ENTRY:mainCRTStartup")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})
