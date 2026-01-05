file(GLOB Log ${CMAKE_CURRENT_SOURCE_DIR}/log/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/log/*.h)
file(GLOB_RECURSE Gerber ${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/*.h)

if (WIN32)
	set(Prefix windows)
elseif (APPLE)
	set(Prefix macos)
elseif (UNIX)
	set(Prefix ubuntu)
endif ()

add_library(gerber_parser SHARED ${Gerber} ${Log})
target_include_directories(
	gerber_parser PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/engine>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
	$<INSTALL_INTERFACE:${Prefix}/include>
)
# Remove license dependency
if (WIN32)
else ()
	target_compile_options(gerber_parser PRIVATE -fPIC)
	set_target_properties(gerber_parser PROPERTIES LINK_FLAGS -fvisibility=hidden)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Gerber} ${Log})

install(TARGETS gerber_parser
	EXPORT gerber_parser
	ARCHIVE DESTINATION ${Prefix}/lib
	LIBRARY DESTINATION ${Prefix}/lib
	RUNTIME DESTINATION ${Prefix}/bin
)

set(InstallHeader1
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/bound_box.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/gerber_parser.h"
)
set(InstallHeader2
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/engine.h"
)
set(InstallHeader3
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/gerber/gerber.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/gerber/gerber_layer.h"
)
set(InstallHeader4
	"${CMAKE_CURRENT_SOURCE_DIR}/gerber_parser/aperture/aperture.h"
)
install(FILES ${InstallHeader1} DESTINATION ${Prefix}/include/gerber_parser)
install(FILES ${InstallHeader2} DESTINATION ${Prefix}/include/engine)
install(FILES ${InstallHeader3} DESTINATION ${Prefix}/include/gerber)
install(FILES ${InstallHeader4} DESTINATION ${Prefix}/include/aperture)

set(Version 1.0.0)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        gerber_parser/cmake/gerber_parserConfigVersion.cmake
        VERSION ${Version}
        COMPATIBILITY AnyNewerVersion
)
install(EXPORT gerber_parser
	FILE gerber_parserTargets.cmake
	DESTINATION ${Prefix}/cmake
)

configure_file(gerber_parser-config.cmake.in gerber_parser-config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gerber_parser-config.cmake"
        DESTINATION ${Prefix}/cmake
)

set(CPACK_INSTALL_PREFIX "")
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "gerber_parser")
set(CPACK_SET_DESTDIR ON)
set(CPACK_PACKAGE_VERSION "1.0.0") 
include(CPack)