# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

file(GLOB_RECURSE
	SourceFiles
	"${PROJECT_SOURCE_DIR}/src/gerber_parser/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_parser/*.h"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.cpp"
	"${PROJECT_SOURCE_DIR}/src/gerber_renderer.h"
	"${PROJECT_SOURCE_DIR}/src/log/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/log/*.h"
    "${PROJECT_SOURCE_DIR}/src/permission/*.cpp"
    main.cpp
)

add_executable(example ${SourceFiles})
target_include_directories(
	example PRIVATE	
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/gerber_parser"
)
target_link_libraries(
	example PRIVATE
	license
)
target_compile_definitions(example PRIVATE TestData="${CMAKE_SOURCE_DIR}/tests/test_data/gerber/" GerberApi=)
