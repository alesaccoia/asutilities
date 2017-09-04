# MIT License

# Copyright (c) 2017 Alessandro Saccoia <alessandro@mogees.co.uk>
# Copyright (c) 2014 Alessandro Saccoia <alessandro@dinahmoe.com>

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#


CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT("asutilities")

MESSAGE(STATUS "Configuring asutilities")

OPTION(APPLE_DONT_USE_QUICKTIME "Excludes core audio but uses the rest on apple" OFF)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
INCLUDE(SetTargetOptions)
INCLUDE(SetPlatformDefines)
SET_PLATFORM_DEFINES()
SET(compile_options "")

IF(DEFINED USE_SAMPLERATE)
  INCLUDE(FindSampleRate)
  MESSAGE(STATUS "****************** Using LibSampleRate -> GPL LICENSE! *******************")
  SET(compile_options "${compile_options} -DASUTILITIES_USE_SAMPLERATE=1 ")
ENDIF()

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include/")                         
FILE(GLOB include_f "${CMAKE_CURRENT_SOURCE_DIR}/include/*.*")

SET(ASUTILITIES_LINK_FLAGS "" CACHE INTERNAL "Linker flags for dm_audiomidi" )

### CoreAudio ### <- Quicktime, Coreaudio formats
IF (APPLE AND NOT APPLE_DONT_USE_QUICKTIME)
  MESSAGE(STATUS "asutilities: Using Apple quicktime libraries")
  LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_CoreAudio.hpp)
  LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_CoreAudio.cpp)
  SET(ASUTILITIES_LINK_FLAGS "${ASUTILITIES_LINK_FLAGS} -framework AudioToolbox -framework CoreFoundation" CACHE INTERNAL "Linker flags for dm_audiomidi" )
  SET(compile_options "${compile_options} -DASUTILITIES_USE_COREAUDIO=1 ")
ENDIF(APPLE AND NOT APPLE_DONT_USE_QUICKTIME)

### OGG ### <- always added
MESSAGE(STATUS "asutilities: Using Ogg Decoder -> PUBLIC DOMAIN LICENSE")
LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.cpp)
LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.hpp)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.cpp PROPERTIES COMPILE_FLAGS "-Wno-unused-value -Wno-sign-compare -Wno-all")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/stb_vorbis.c PROPERTIES COMPILE_FLAGS "-Wno-unused-value -Wno-sign-compare -Wno-all")
SET(compile_options "${compile_options} -DASUTILITIES_USE_OGG=1 ")

### AAC ###
IF ((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME)
  MESSAGE(STATUS "asutilities: Using FDK AAc -> MIT-STYLE LICENSE")
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libAACdec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libAACenc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libFDK/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libMpegTPDec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libMpegTPEnc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libPCMutils/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSBRdec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSBRenc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSYS/include)
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/ "fdk-aac")
  LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_aac.cpp)
  LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_aac.hpp)
  SET(compile_options "${compile_options} -DASUTILITIES_USE_AAC=1 ")
  SET(ASUTILITIES_EXTRA_LIBS ${ASU_LIBFDK_AAC_LIBS} CACHE INTERNAL "Libraries for asutilities")
ENDIF ((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME)

### SNDFILE ###
IF ((NOT ANDROID) AND((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME))
  INCLUDE(FindSndFile)
  IF (SNDFILE_FOUND)
    MESSAGE(STATUS "asutilities: Using Libsndfile -> LGPL License")
    INCLUDE_DIRECTORIES(${SNDFILE_INCLUDE_DIRS})
    IF (WIN32)
      SET(ASUTILITIES_LINK_FLAGS "${ASUTILITIES_LINK_FLAGS} -lsndfile" CACHE INTERNAL "Linker flags for dm_audiomidi" )
    ELSE(WIN32)
      SET(ASUTILITIES_LINK_FLAGS "${ASUTILITIES_LINK_FLAGS} ${SNDFILE_LIBRARIES}" CACHE INTERNAL "Linker flags for dm_audiomidi" )
    ENDIF(WIN32)
    LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_sndfile.cpp)
    LIST(APPEND ASUTILITIES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_sndfile.hpp)
    SET(compile_options "${compile_options} -DASUTILITIES_USE_SNDFILE=1 ")
    MESSAGE(${ASUTILITIES_LINK_FLAGS})
  ELSE(SNDFILE_FOUND)
    MESSAGE(FATAL_ERROR "Libsoundfile not found, don't use FORCE_SOUNDFILE")
  ENDIF(SNDFILE_FOUND)
ENDIF ((NOT ANDROID) AND((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME))

# INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../dm-utils/include)

MESSAGE(STATUS ${compile_options})

ADD_LIBRARY(asutilities
    ${include_f}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormatsManager.cpp
    ${ASUTILITIES_SRCS})
# TARGET_LINK_LIBRARIES(asutilities ${ASUTILITIES_EXTRA_LIBS})

FSET_TARGET_OPTIONS(asutilities ${compile_options})

ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
