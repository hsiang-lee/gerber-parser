# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package(GTest REQUIRED)

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
	"${PROJECT_SOURCE_DIR}/src/gerber_render.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_render.h"
	"${PROJECT_SOURCE_DIR}/src/engine/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/engine/*.h"
)

add_executable(TestGerberRender ${TestSrc} ${SourceFiles})
target_include_directories(
	TestGerberRender PRIVATE	
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/gerber"
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(
	TestGerberRender PRIVATE
	${GTEST_BOTH_LIBRARIES}
	GTest::gmock
	GTest::gmock_main
	Qt5::Core
	Qt5::Widgets
	Qt5::Gui
	glog::glog
)
target_compile_definitions(TestGerberRender PRIVATE TestData="${CMAKE_CURRENT_SOURCE_DIR}/test_data/")

source_group(TREE ${PROJECT_SOURCE_DIR}/tests FILES ${TestSrc})
source_group(TREE ${PROJECT_SOURCE_DIR}/src FILES ${SourceFiles})

add_test(TestGerberRender TestGerberRender)
