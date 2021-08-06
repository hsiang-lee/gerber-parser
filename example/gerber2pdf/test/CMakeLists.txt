# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE
	TestSrc
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE
	SourceFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/pdf/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/pdf/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf/pdf/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf/pdf/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf/afm/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf/afm/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf_engine/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf_engine/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/../tools/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../tools/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/../deflate/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../deflate/*.h"
)

add_executable(TestGerber2pdf ${TestSrc} ${SourceFiles})
target_include_directories(
	TestGerber2pdf PRIVATE	
	"${CMAKE_CURRENT_SOURCE_DIR}/.."
	"${CMAKE_CURRENT_SOURCE_DIR}/../pdf"
)
target_link_libraries(
	TestGerber2pdf PRIVATE
	${GTEST_BOTH_LIBRARIES}
	gtest
	gtest_main
	gerber_render
)
target_compile_definitions(TestGerber2pdf PRIVATE TestData="${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/test_data/")

add_test(TestGerber2pdf TestGerber2pdf)
