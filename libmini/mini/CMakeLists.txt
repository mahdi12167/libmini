PROJECT(libMini)

cmake_minimum_required(VERSION 2.4)

# Unix compiler definitions
IF (UNIX)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
ENDIF (UNIX)

# Windows compiler definitions
IF (WIN32)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305")
   ADD_DEFINITIONS(-DPTW32_STATIC_LIB -DOT_LIBRARY_STATIC -DCURL_STATICLIB -DFREEGLUT_STATIC)
ENDIF (WIN32)

# find common dependencies
FIND_PACKAGE(OpenGL)
IF (UNIX)
   FIND_PACKAGE(Threads)
   SET(THREAD_LIBRARY ${PTHREAD_LIBRARY})
ELSE (UNIX)
   FIND_PACKAGE(OpenThreads)
   ADD_DEFINITIONS(-DUSEOPENTH)
   SET(THREAD_LIBRARY ${OPENTHREAD_LIBRARY})
ENDIF (UNIX)
FIND_PACKAGE(CURL)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(PNG)
FIND_PACKAGE(GLUT)

# find Squish dependency
FIND_LIBRARY(SQUISH_LIBRARY squish "$ENV{SQUISH_DIR}")
FIND_PATH(SQUISH_INCLUDE_DIR squish.h "$ENV{SQUISH_DIR}")
INCLUDE_DIRECTORIES("${SQUISH_INCLUDE_DIR}")

# find GREYCstoration dependency
IF (UNIX AND NOT APPLE)
   FIND_PATH(GREYCSTORATION_INCLUDE_DIR CImg.h "$ENV{GREYCSTORATION_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../deps/greycstoration")
   IF (GREYCSTORATION_INCLUDE_DIR)
      INCLUDE_DIRECTORIES("${GREYCSTORATION_INCLUDE_DIR}")
      ADD_DEFINITIONS(-DUSEGREYC)
   ENDIF (GREYCSTORATION_INCLUDE_DIR)
ENDIF (UNIX AND NOT APPLE)

# libMini sources:

SET(MINI_HDRS
   mini.h miniOGL.h
   minibase.h minidyna.h minisort.h
   miniv3f.h miniv3d.h miniv4f.h miniv4d.h
   minimath.h minimpfp.h minitime.h miniio.h minihsv.h miniutm.h
   ministub.h minitile.h miniload.h
   minicoord.h miniwarp.h
   minilayer.h miniterrain.h miniearth.h
   minicache.h minishader.h miniray.h
   ministrip.h minipoint.h minitext.h minisky.h miniglobe.h
   minitree.h minibrick.h minilod.h
   minigeom.h minimesh.h minibspt.h miniproj.h
   pnmbase.h pnmsample.h
   database.h datafill.h
   datacloud.h datacache.h datagrid.h
   datacalc.h dataparse.h
   lunascan.h lunaparse.h lunacode.h
   )

SET(MINI_SRCS
   mini.cpp miniOGL.cpp
   miniv3f.cpp miniv3d.cpp miniv4f.cpp miniv4d.cpp
   minimath.cpp minimpfp.cpp minitime.cpp miniio.cpp minihsv.cpp miniutm.cpp
   ministub.cpp minitile.cpp miniload.cpp
   minicoord.cpp miniwarp.cpp
   minilayer.cpp miniterrain.cpp miniearth.cpp
   minicache.cpp minishader.cpp miniray.cpp
   ministrip.cpp minipoint.cpp minitext.cpp minisky.cpp miniglobe.cpp
   minitree.cpp minibrick.cpp minilod.cpp
   minigeom.cpp minimesh.cpp minibspt.cpp miniproj.cpp
   pnmbase.cpp pnmsample.cpp
   database.cpp datafill.cpp
   datacloud.cpp datacache.cpp datagrid.cpp
   datacalc.cpp dataparse.cpp
   lunascan.cpp lunaparse.cpp lunacode.cpp
   )

SET(MINISFX_SRCS
   viewerbase.cpp
   threadbase.cpp curlbase.cpp
   jpegbase.cpp pngbase.cpp squishbase.cpp greycbase.cpp
   convbase.cpp imgbase.cpp
   panorndr.cpp
   )

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")

ADD_LIBRARY(mini ${MINI_SRCS})
TARGET_LINK_LIBRARIES(mini ${OPENGL_LIBRARIES})

ADD_LIBRARY(miniSFX ${MINISFX_SRCS})
ADD_DEPENDENCIES(miniSFX mini)
TARGET_LINK_LIBRARIES(miniSFX
   mini
   ${THREAD_LIBRARY}
   ${CURL_LIBRARY}
   ${JPEG_LIBRARY}
   ${PNG_LIBRARY}
   ${SQUISH_LIBRARY}
   )

MACRO(MAKE_EXECUTABLE name)
   ADD_EXECUTABLE(${name} ${name}.cpp)
   TARGET_LINK_LIBRARIES(${name}
      mini
      miniSFX
      ${THREAD_LIBRARY}
      ${CURL_LIBRARY}
      ${JPEG_LIBRARY}
      ${PNG_LIBRARY}
      ${SQUISH_LIBRARY}
      ${GLUT_LIBRARY}
      )
ENDMACRO(MAKE_EXECUTABLE)

MAKE_EXECUTABLE(example)
MAKE_EXECUTABLE(viewer)

ADD_SUBDIRECTORY(tools)

INSTALL(
   TARGETS mini miniSFX viewer
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   RUNTIME DESTINATION bin
   )

INSTALL(
   FILES ${MINI_HDRS}
   DESTINATION include/mini
   )
