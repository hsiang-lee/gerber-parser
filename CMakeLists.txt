cmake_minimum_required(VERSION 3.20)

project(gerber_render C CXX)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS OFF)
option(BUILD_EXAMPLES OFF)

add_subdirectory(3rdparty/glog)
target_compile_definitions(glog PRIVATE "HAVE_SNPRINTF")
add_subdirectory(src)

if(BUILD_EXAMPLES)
	add_subdirectory(3rdparty/gflags)
	
	add_subdirectory(example/gerber2pdf)
	add_subdirectory(example/gerber2image)
	add_subdirectory(example/gerber_viewer)
endif()

if(BUILD_TESTS)
	add_subdirectory(3rdparty/googletest)

	enable_testing()
	add_subdirectory(tests)
endif()
