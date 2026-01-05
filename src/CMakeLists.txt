find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

# Add parser/src subdirectory to build gerber_parser library
add_subdirectory(parser/src)

file(GLOB Engines ${CMAKE_CURRENT_SOURCE_DIR}/engines/*.cpp)
add_library(gerber_engine STATIC ${Engines})
target_include_directories(
	gerber_engine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/parser/src
	${CMAKE_CURRENT_SOURCE_DIR}/parser/src/gerber_parser
)
target_link_libraries(gerber_engine PUBLIC Qt6::Core Qt6::Widgets Qt6::Gui gerber_parser)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Engines})

add_subdirectory(pygerber-parser)