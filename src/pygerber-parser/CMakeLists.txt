find_package(Qt6 COMPONENTS Core Widgets  Gui OpenGLWidgets REQUIRED)
set(CMAKE_AUTORCC ON)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(pygerber-parser SHARED ${Source})
target_link_libraries(pygerber-parser PRIVATE gerber_engine Qt6::Gui Qt6::Widgets Qt6::Core)