find_package(PkgConfig)
pkg_check_modules(PC_LIBJACK QUIET libjack)
set(LIBJACK_DEFINITIONS ${PC_LIBJACK_CFLAGS_OTHER})

find_path(LIBJACK_INCLUDE_DIR jack/jack.h
          HINTS ${PC_LIBJACK_INCLUDEDIR} ${PC_LIBJACK_INCLUDE_DIRS}
          PATH_SUFFIXES libjack )

find_library(LIBJACK_LIBRARY NAMES jack libjack
             HINTS ${PC_LIBJACK_LIBDIR} ${PC_LIBJACK_LIBRARY_DIRS} )

set(LIBJACK_LIBRARIES ${LIBJACK_LIBRARY} )
set(LIBJACK_INCLUDE_DIRS ${LIBJACK_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBJACK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibJack  DEFAULT_MSG
                                  LIBJACK_LIBRARY LIBJACK_INCLUDE_DIR)

mark_as_advanced(LIBJACK_INCLUDE_DIR LIBJACK_LIBRARY )
