# -------------------------------------------------------------------------------------------
# this file declares two function used to create the right Xcode targets for both OsX and iOs
# with support for c++0x, so deployment target Mac OS >= 10.7, iOs >= 5.1
#
# argument: the name of the target
#
# Note: Requires the new-ish Xcode installed in /Applications, not in /Developer
#
# Author: Alessandro Saccoia <alessandro@dinahmoe.com>
# --------------------------------------------------------------------------------------------
#
# Various how tos:
# CC=/Applications/Xcode.app/Contents/Developer/usr/bin/gcc CXX=/Applications/Xcode.app/Contents/Developer/usr/bin/gcc cmake .. -DBUILD_FOR:STRING="ios" -DIOS_ARCH="simulator"
# this compiles with the newest apple compiler. sometimes you get errors from xcode if using an obj project and a library compiled with different versions of the apple compilers.
#
# lipo -output universal.a -create lib_x86_64.a lib_armv7.a
# creates a fat library
 
FUNCTION(FSET_TARGET_OPTIONS MODULE_NAME OTHER_FLAGS)
  IF(APPLE)
    IF("${BUILD_FOR}" STREQUAL "ios" )   
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        SET(CMAKE_OSX_SYSROOT "")
        IF("${IOS_ARCH}" STREQUAL "arm" )
          SET(possible_sdk_roots
            /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs
            /Developer/Platforms/iPhoneOS.platform/Developer/SDKs
          )
          FOREACH(sdk_root ${possible_sdk_roots})
            FOREACH(sdk iPhoneOS5.1.sdk iPhoneOS6.0.sdk iPhoneOS6.1.sdk iPhoneOS7.1.sdk iPhoneOS8.4.sdk iPhoneOS9.0.sdk iPhoneOS9.1.sdk iPhoneOS.sdk)
              IF (EXISTS ${sdk_root}/${sdk} AND IS_DIRECTORY ${sdk_root}/${sdk})
                SET(CMAKE_OSX_SYSROOT ${sdk_root}/${sdk})
              ENDIF()
            ENDFOREACH()
          ENDFOREACH()
          IF(CMAKE_OSX_SYSROOT STREQUAL "" )
            MESSAGE(FATAL_ERROR "Could not find a usable iOS SDK in ${sdk_root}")
          ENDIF()
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "iphoneos")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
          SET (IOS_ARCH_FLAG "-arch armv6 -arch armv7 -arch armv7s -miphoneos-version-min=5.0")
        ELSEIF("${IOS_ARCH}" STREQUAL "simulator" )
          SET(possible_sdk_roots
            /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs
            /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs
          )
          FOREACH(sdk_root ${possible_sdk_roots})
            FOREACH(sdk iPhoneSimulator5.1.sdk iPhoneSimulator6.0.sdk iPhoneSimulator6.1.sdk iPhoneSimulator7.1.sdk iPhoneSimulator8.4.sdk iPhoneSimulator9.0.sdk iPhoneSimulator9.1.sdk iPhoneSimulator.sdk)
              IF (EXISTS ${sdk_root}/${sdk} AND IS_DIRECTORY ${sdk_root}/${sdk})
                SET(CMAKE_OSX_SYSROOT ${sdk_root}/${sdk})
              ENDIF()
            ENDFOREACH()
          ENDFOREACH()
          IF(CMAKE_OSX_SYSROOT STREQUAL "" )
            MESSAGE(FATAL_ERROR "Could not find a usable iOS SDK in ${sdk_root}")
          ENDIF()
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "iphonesimulator")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
          # @#@!#!@# AHHH YES! see
          # http://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art024
          SET (IOS_ARCH_FLAG "-mios-simulator-version-min=7.1 -arch i386")
        ENDIF()
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} ${IOS_ARCH_FLAG} -x objective-c++ -std=c++0x -stdlib=libc++ -Wall -isysroot ${CMAKE_OSX_SYSROOT}")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles")  
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++ -O3 -std=c++0x -stdlib=libc++ -Wall")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "iphoneos")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD_32_BIT)")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ELSEIF("${BUILD_FOR}" STREQUAL "tvos" )   
      MESSAGE(STATUS "Building for TVOS, again")
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        IF(${TVOS_ARCH} STREQUAL "arm" )
          SET(CMAKE_OSX_SYSROOT /Applications/Xcode-beta.app/Contents/Developer/Platforms/AppleTVOS.platform/Developer/SDKs/AppleTVOS.sdk)
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "appletvos")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL)")
          SET (TVOS_ARCH_FLAG "-arch arm64")
        ELSEIF(${TVOS_ARCH} STREQUAL "simulator" )
          SET(CMAKE_OSX_SYSROOT /Applications/Xcode-beta.app/Contents/Developer/Platforms/AppleTVSimulator.platform/Developer/SDKs/AppleTVSimulator.sdk)
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "appletvsimulator")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL)")
          # @#@!#!@# AHHH YES! see
          # http://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art024
          SET (TVOS_ARCH_FLAG "-mtvos-simulator-version-min=9.0 -arch x86_64")
        ENDIF()
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} ${TVOS_ARCH_FLAG} -x objective-c++ -std=c++0x -stdlib=libc++ -Wall -isysroot ${CMAKE_OSX_SYSROOT}")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles")  
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++ -O3 -std=c++0x -stdlib=libc++ -Wall")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "appletvos")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD)")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ELSEIF("${BUILD_FOR}" STREQUAL "osx")	 
      SET(CMAKE_OSX_SYSROOT "")   
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -O3 -std=c++11 -stdlib=libc++ -Wall")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles") 
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++ -O3 -std=c++0x -stdlib=libc++ -Wall")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "macosx")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ENDIF()
  ELSEIF(WIN32) # cygwin, same as linux
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -std=c++11 -stdlib=libc++ -Wall -Wno-unknown-pragmas")
  ELSEIF(ANDROID)
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -std=c++11  -Wall -Wno-unknown-pragmas")
  ELSE()
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -pthread -std=c++11 -Wall -Wno-unknown-pragmas")
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES LINK_FLAGS " -lpthread")
  ENDIF()
ENDFUNCTION()


FUNCTION(FSET_TARGET_OPTIONS_NO_CPP11 MODULE_NAME OTHER_FLAGS)
  IF(APPLE)
    IF("${BUILD_FOR}" STREQUAL "ios" )
      SET(CMAKE_OSX_SYSROOT "")
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        IF("${IOS_ARCH}" STREQUAL "arm" )
          SET(possible_sdk_roots
            /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs
            /Developer/Platforms/iPhoneOS.platform/Developer/SDKs
          )
          FOREACH(sdk_root ${possible_sdk_roots})
            FOREACH(sdk iPhoneOS5.1.sdk iPhoneOS6.0.sdk iPhoneOS6.1.sdk iPhoneOS7.1.sdk iPhoneOS8.4.sdk iPhoneOS9.0.sdk iPhoneOS9.1.sdk iPhoneOS.sdk)
              IF (EXISTS ${sdk_root}/${sdk} AND IS_DIRECTORY ${sdk_root}/${sdk})
                SET(CMAKE_OSX_SYSROOT ${sdk_root}/${sdk})
              ENDIF()
            ENDFOREACH()
          ENDFOREACH()
          IF(CMAKE_OSX_SYSROOT STREQUAL "" )
            MESSAGE(FATAL_ERROR "Could not find a usable iOS SDK in ${sdk_root}")
          ENDIF()
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
          SET (IOS_ARCH_FLAG "-arch armv6 -arch armv7 -arch armv7s -miphoneos-version-min=5.0")
        ELSEIF("${IOS_ARCH}" STREQUAL "simulator" )
          SET(possible_sdk_roots
            /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs
            /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs
          )
          FOREACH(sdk_root ${possible_sdk_roots})
            FOREACH(sdk iPhoneSimulator5.1.sdk iPhoneSimulator6.0.sdk iPhoneSimulator6.1.sdk iPhoneSimulator7.1.sdk  iPhoneSimulator8.4.sdk iPhoneSimulator9.0.sdk iPhoneSimulator9.1.sdk iPhoneSimulator.sdk)
              IF (EXISTS ${sdk_root}/${sdk} AND IS_DIRECTORY ${sdk_root}/${sdk})
                SET(CMAKE_OSX_SYSROOT ${sdk_root}/${sdk})
              ENDIF()
            ENDFOREACH()
          ENDFOREACH()
          IF(CMAKE_OSX_SYSROOT STREQUAL "" )
            MESSAGE(FATAL_ERROR "Could not find a usable iOS SDK in ${sdk_root}")
          ENDIF()
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
          # @#@!#!@# AHHH YES! see
          # http://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art024
          SET (IOS_ARCH_FLAG "-mios-simulator-version-min=7.1 -arch i386")
        ENDIF()
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} ${IOS_ARCH_FLAG} -x objective-c++ -std=c++0x -stdlib=libc++ -Wall -isysroot ${CMAKE_OSX_SYSROOT}")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles")  
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++ -O3 -std=c++0x -stdlib=libc++ -Wall")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "iphoneos")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD_32_BIT)")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ELSEIF("${BUILD_FOR}" STREQUAL "tvos" )   
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        SET(CMAKE_OSX_SYSROOT "")
        IF("${TVOS_ARCH}" STREQUAL "arm" )
          SET(CMAKE_OSX_SYSROOT /Applications/Xcode-beta.app/Contents/Developer/Platforms/AppleTVOS.platform/Developer/SDKs/AppleTVOS.sdk)
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "appletvos")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL)")
          SET (TVOS_ARCH_FLAG "-arch arm64")
        ELSEIF("${TVOS_ARCH}" STREQUAL "simulator" )
          SET(CMAKE_OSX_SYSROOT /Applications/Xcode-beta.app/Contents/Developer/Platforms/AppleTVSimulator.platform/Developer/SDKs/AppleTVSimulator.sdk)
          SET(CMAKE_OSX_DEPLOYMENT_TARGET "appletvsimulator")
          SET(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL)")
          # @#@!#!@# AHHH YES! see
          # http://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art024
          SET (TVOS_ARCH_FLAG "-mtvos-simulator-version-min=9.0 -arch x86_64")
        ENDIF()
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} ${TVOS_ARCH_FLAG} -x objective-c++ -std=c++0x -stdlib=libc++ -Wall -isysroot ${CMAKE_OSX_SYSROOT}")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles")  
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++ -O3 -std=c++0x -stdlib=libc++ -Wall")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "appletvos")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD)")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ELSEIF("${BUILD_FOR}" STREQUAL "osx")	
      IF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} ")
        TARGET_LINK_LIBRARIES(${MODULE_NAME} "c++")
      ELSE(CMAKE_GENERATOR MATCHES "^Unix Makefiles")   
        SET(CMAKE_OSX_SYSROOT "")        
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -x objective-c++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_SDKROOT "macosx")
      ENDIF(CMAKE_GENERATOR MATCHES "^Unix Makefiles")
    ENDIF()
  ELSEIF(WIN32) # cygwin, same as linux
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -stdlib=libc++ -Wall -Wno-unknown-pragmas")
  ELSEIF(ANDROID)
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -Wall -Wno-unknown-pragmas")
  ELSE()
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES COMPILE_FLAGS "${OTHER_FLAGS} -pthread -Wall -Wno-unknown-pragmas")
    SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES LINK_FLAGS " -lpthread")
  ENDIF()
ENDFUNCTION()

