find_package(Qt6 COMPONENTS Core Widgets Gui Widgets OpenGLWidgets REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(gerber_viewer ${Source})
target_link_libraries(gerber_viewer PRIVATE gerber_engine Qt6::OpenGLWidgets)

if(WIN32)
target_link_options(gerber_viewer PRIVATE "/ENTRY:mainCRTStartup" "/SUBSYSTEM:WINDOWS")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source})