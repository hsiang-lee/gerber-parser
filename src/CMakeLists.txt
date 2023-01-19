find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

file(GLOB Engines ${CMAKE_CURRENT_SOURCE_DIR}/engines/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/engines/*.h)
add_library(gerber_engine STATIC ${Engines})
target_include_directories(
	gerber_engine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(gerber_engine PUBLIC gerber_parser Qt5::Core Qt5::Widgets Qt5::Gui)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Engine})
