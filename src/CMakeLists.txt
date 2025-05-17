find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

file(GLOB Engines ${CMAKE_CURRENT_SOURCE_DIR}/engines/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)
add_library(gerber_engine STATIC ${Engines})
target_include_directories(
	gerber_engine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(gerber_engine PUBLIC gerber_parser Qt6::Core Qt6::Widgets Qt6::Gui)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Engine})

#add_subdirectory(pygerber-parser)
