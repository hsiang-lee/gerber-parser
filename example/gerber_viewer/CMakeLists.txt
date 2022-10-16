find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL REQUIRED)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(gerber_viewer ${Source})
target_link_libraries(gerber_viewer PRIVATE glog::glog gerber_parser Qt5::OpenGL)

if(WIN32)
target_link_options(gerber_viewer PRIVATE "/ENTRY:mainCRTStartup" "/SUBSYSTEM:WINDOWS")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})
