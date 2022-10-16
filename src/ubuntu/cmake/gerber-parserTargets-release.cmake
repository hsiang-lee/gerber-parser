#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gerber_parser" for configuration "Release"
set_property(TARGET gerber_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gerber_parser PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "/home/runner/work/gerber_parser/gerber_parser/build/install/gerber_parser/lib/libgerber_parser.a"
  )

list(APPEND _cmake_import_check_targets gerber_parser )
list(APPEND _cmake_import_check_files_for_gerber_parser "/home/runner/work/gerber_parser/gerber_parser/build/install/gerber_parser/lib/libgerber_parser.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
