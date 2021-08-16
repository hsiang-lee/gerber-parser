file(GLOB Main ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB PdfEngine ${CMAKE_CURRENT_SOURCE_DIR}/pdf_engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pdf_engine/*.h)
file(GLOB Deflate ${CMAKE_CURRENT_SOURCE_DIR}/deflate/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/deflate/*.h)
file(GLOB Tools ${CMAKE_CURRENT_SOURCE_DIR}/tools/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tools/*.h)
file(GLOB_RECURSE Pdf ${CMAKE_CURRENT_SOURCE_DIR}/pdf/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pdf/*.h)

add_executable(gerber2pdf ${Main} ${PdfEngine} ${Pdf} ${Tools} ${Deflate})
target_link_libraries(gerber2pdf PRIVATE gerber_renderer gflags)
target_include_directories(gerber2pdf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Source} ${PdfEngine} ${Pdf} ${Tools} ${Deflate})

add_subdirectory(test)
