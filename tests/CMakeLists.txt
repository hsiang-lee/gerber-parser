# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE
	TestSrc
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(TestGerberRenderer ${TestSrc})
target_link_libraries(
	TestGerberRenderer PRIVATE
	gerber_engine
	gtest
	gtest_main
	Qt5::Core
	Qt5::Widgets
	Qt5::Gui
)
target_compile_definitions(TestGerberRenderer PRIVATE TestData="${CMAKE_CURRENT_SOURCE_DIR}/test_data/gerber/")

source_group(TREE ${PROJECT_SOURCE_DIR}/tests FILES ${TestSrc})
source_group(TREE ${PROJECT_SOURCE_DIR}/src FILES ${SourceFiles})

add_test(TestGerberRenderer TestGerberRenderer)
