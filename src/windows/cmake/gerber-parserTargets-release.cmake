#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gerber_parser" for configuration "Release"
set_property(TARGET gerber_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gerber_parser PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/windows/lib/gerber_parser.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/windows/bin/gerber_parser.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS gerber_parser )
list(APPEND _IMPORT_CHECK_FILES_FOR_gerber_parser "${_IMPORT_PREFIX}/windows/lib/gerber_parser.lib" "${_IMPORT_PREFIX}/windows/bin/gerber_parser.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
