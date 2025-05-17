# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

file(GLOB_RECURSE
	TestSrc
	"${CMAKE_CURRENT_SOURCE_DIR}/permission/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/*.cpp"
)
file(GLOB_RECURSE
	SourceFiles
	"${PROJECT_SOURCE_DIR}/src/gerber_parser/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_parser/*.h"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.h"
	"${PROJECT_SOURCE_DIR}/src/log/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/log/*.h"
)

add_executable(TestGerberParser ${TestSrc} ${SourceFiles})
target_include_directories(
	TestGerberParser PRIVATE	
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/gerber_parser"
)
target_link_libraries(
	TestGerberParser PRIVATE
	license
	gtest
	gtest_main
)
target_compile_definitions(TestGerberParser PRIVATE TestData="${CMAKE_CURRENT_SOURCE_DIR}/test_data/gerber/" LicenseData="${CMAKE_CURRENT_SOURCE_DIR}/test_data/license/" GerberApi=)

source_group(TREE ${PROJECT_SOURCE_DIR}/tests FILES ${TestSrc})
source_group(TREE ${PROJECT_SOURCE_DIR}/src FILES ${SourceFiles})

add_test(TestGerberParser TestGerberParser)
