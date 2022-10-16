#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gerber_parser" for configuration "Debug"
set_property(TARGET gerber_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gerber_parser PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/macos/lib/libgerber_parser.dylib"
  IMPORTED_SONAME_DEBUG "@rpath/libgerber_parser.dylib"
  )

list(APPEND _cmake_import_check_targets gerber_parser )
list(APPEND _cmake_import_check_files_for_gerber_parser "${_IMPORT_PREFIX}/macos/lib/libgerber_parser.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)