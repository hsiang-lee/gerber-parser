# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE
	TestSrc
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE
	SourceFiles
	"${PROJECT_SOURCE_DIR}/src/gerber/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber/*.h"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.h"
	"${PROJECT_SOURCE_DIR}/src/engine/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/engine/*.h"
)

add_executable(TestGerberRenderer ${TestSrc} ${SourceFiles})
target_include_directories(
	TestGerberRenderer PRIVATE	
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/gerber"
)
target_link_libraries(
	TestGerberRenderer PRIVATE
	gtest
	gmock
	gmock_main
	Qt5::Core
	Qt5::Widgets
	Qt5::Gui
	glog::glog
)
target_compile_definitions(TestGerberRenderer PRIVATE TestData="${CMAKE_CURRENT_SOURCE_DIR}/test_data/gerber/")

source_group(TREE ${PROJECT_SOURCE_DIR}/tests FILES ${TestSrc})
source_group(TREE ${PROJECT_SOURCE_DIR}/src FILES ${SourceFiles})

add_test(TestGerberRenderer TestGerberRenderer)
