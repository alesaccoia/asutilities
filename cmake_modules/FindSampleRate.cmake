# - Try to find SamplingRate
# Once done this will define
#
#  SAMPLERATE_FOUND - system has Samplerate
#  SAMPLERATE_INCLUDE_DIRS - the Samplerate include directory
#  SAMPLERATE_LIBRARIES - Link these to use Samplerate
#  SAMPLERATE_DEFINITIONS - Compiler switches required for using Samplerate
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (SAMPLERATE_LIBRARIES AND SAMPLERATE_INCLUDE_DIRS)
  # in cache already
  set(SAMPLERATE_FOUND TRUE)
else (SAMPLERATE_LIBRARIES AND SAMPLERATE_INCLUDE_DIRS)
  find_path(SAMPLERATE_INCLUDE_DIR
    NAMES
      samplerate.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(SAMPLERATE_LIBRARY
    NAMES
      samplerate
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(SAMPLERATE_INCLUDE_DIRS
    ${SAMPLERATE_INCLUDE_DIR}
  )
  set(SAMPLERATE_LIBRARIES
    ${SAMPLERATE_LIBRARY}
  )

  if (SAMPLERATE_INCLUDE_DIRS AND SAMPLERATE_LIBRARIES)
     set(SAMPLERATE_FOUND TRUE)
  endif (SAMPLERATE_INCLUDE_DIRS AND SAMPLERATE_LIBRARIES)

  if (SAMPLERATE_FOUND)
    if (NOT Samplerate_FIND_QUIETLY)
      message(STATUS "Found Samplerate: ${SAMPLERATE_LIBRARIES}")
    endif (NOT Samplerate_FIND_QUIETLY)
  else (SAMPLERATE_FOUND)
    if (Samplerate_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Samplerate")
    endif (Samplerate_FIND_REQUIRED)
  endif (SAMPLERATE_FOUND)

  # show the SAMPLERATE_INCLUDE_DIRS and SAMPLERATE_LIBRARIES variables only in the advanced view
  mark_as_advanced(SAMPLERATE_INCLUDE_DIRS SAMPLERATE_LIBRARIES)

endif (SAMPLERATE_LIBRARIES AND SAMPLERATE_INCLUDE_DIRS)