find_path(MSGPUCK_INCLUDEDIR msgpuck.h
  PATH_SUFFIXES msgpuck
)

find_library(MSGPUCK_LIBRARIES
    NAMES msgpuck
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MSGPUCK REQUIRED_VARS
    MSGPUCK_INCLUDEDIR MSGPUCK_LIBRARIES)
mark_as_advanced(MSGPUCK_INCLUDEDIR MSGPUCK_LIBRARIES)
