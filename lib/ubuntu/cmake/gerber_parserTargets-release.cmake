#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gerber_parser" for configuration "Release"
set_property(TARGET gerber_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gerber_parser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/ubuntu/lib/libgerber_parser.so"
  IMPORTED_SONAME_RELEASE "libgerber_parser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS gerber_parser )
list(APPEND _IMPORT_CHECK_FILES_FOR_gerber_parser "${_IMPORT_PREFIX}/ubuntu/lib/libgerber_parser.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
