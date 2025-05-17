file(GLOB Srcs
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/base64/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/base64/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/md5/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/md5/*.h
)
if (WIN32)
	message(STATUS Windows)
	file(GLOB_RECURSE Platform
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/win/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/win/*.h
	)
	set(MachineInfo ${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/win/)
elseif (APPLE)
	message(STATUS MacOS)
	file(GLOB_RECURSE Platform
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/mac/*.mm
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/mac/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/mac/*.h
	)
	set(MachineInfo ${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/mac/)
elseif (UNIX)
	message(STATUS Unix)
	file(GLOB_RECURSE Platform
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/linux/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/linux/*.h
	)
	set(MachineInfo ${CMAKE_CURRENT_SOURCE_DIR}/machineinfo/linux/)
endif ()

add_library(license ${Srcs} ${Platform})

if(NOT DEFINED LICENSE_VERSION)
    set (LICENSE_VERSION $ENV{DRONE_TAG})
endif()
message(STATUS LICENSE_VERSION=${LICENSE_VERSION})

target_compile_definitions(license PRIVATE
    LICENSE_VERSION="${LICENSE_VERSION}" SODIUM_STATIC)
target_compile_options(license PRIVATE -fPIC)

target_include_directories(license PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/libsodium/include
    ${CMAKE_CURRENT_SOURCE_DIR}/machineinfo
    ${MachineInfo}
)

target_link_libraries(license PRIVATE
    sodium
    nlohmann_json::nlohmann_json
)

if (APPLE)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(CF_LIBRARY CoreFoundation)
	target_link_libraries(license PRIVATE
    		${IOKIT_LIBRARY}
    		${CF_LIBRARY}
	)
endif ()
