#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gerber_parser" for configuration "Debug"
set_property(TARGET gerber_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gerber_parser PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/windows/lib/gerber_parser.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/windows/bin/gerber_parser.dll"
  )

list(APPEND _cmake_import_check_targets gerber_parser )
list(APPEND _cmake_import_check_files_for_gerber_parser "${_IMPORT_PREFIX}/windows/lib/gerber_parser.lib" "${_IMPORT_PREFIX}/windows/bin/gerber_parser.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
