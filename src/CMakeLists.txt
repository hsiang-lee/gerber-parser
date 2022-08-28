find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

file(GLOB Renderer ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB Engine ${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/engine/*.h)
file(GLOB_RECURSE Gerber ${CMAKE_CURRENT_SOURCE_DIR}/gerber/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/gerber/*.h)

add_library(gerber_renderer STATIC ${Engine} ${Gerber} ${Renderer})
target_include_directories(
	gerber_renderer PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine"
	"${CMAKE_CURRENT_SOURCE_DIR}/"
)
target_link_libraries(gerber_renderer PUBLIC glog::glog Qt5::Core Qt5::Widgets Qt5::Gui)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Engine} ${Gerber} ${Renderer})

install(TARGETS gerber_renderer)
