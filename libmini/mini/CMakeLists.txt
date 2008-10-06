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
ENDIF (WIN32)

# paths to optional dependencies
SET(SQUISH_PATH ../deps/squish)
SET(GREYCSTORATION_PATH ../deps/greycstoration)

# paths to WIN32 dependencies
SET(WIN32_PTHREAD_PATH ../WIN32/pthread-win32)
SET(WIN32_OPENTHREADS_PATH ../WIN32/openthreads)
SET(WIN32_CURL_PATH ../WIN32/libcurl)
SET(WIN32_SQUISH_PATH ../WIN32/squish)
SET(WIN32_FREEGLUT_PATH ../WIN32/freeglut)

# find common dependencies
FIND_PACKAGE(OpenGL)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(PNG)

# find GREYCstoration dependency
FIND_PATH(GREYCSTORATION_INCLUDE_DIR CImg.h ${GREYCSTORATION_PATH})
IF (GREYCSTORATION_INCLUDE_DIR)
   INCLUDE_DIRECTORIES("${GREYCSTORATION_INCLUDE_DIR}")
   ADD_DEFINITIONS(-DUSEGREYC)
ENDIF (GREYCSTORATION_INCLUDE_DIR)

# find POSIX threads dependency
IF (UNIX OR GREYCSTORATION_INCLUDE_DIR)
   IF (WIN32)
      FIND_LIBRARY(PTHREAD_LIBRARY pthread_static ${WIN32_PTHREAD_PATH})
      FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h ${WIN32_PTHREAD_PATH})
      INCLUDE_DIRECTORIES("${PTHREAD_INCLUDE_DIR}")
      ADD_DEFINITIONS(-DPTW32_STATIC_LIB)
   ELSE (WIN32)
      FIND_PACKAGE(Threads)
   ENDIF (WIN32)
   SET(THREAD_LIBRARY ${PTHREAD_LIBRARY})
ELSE (UNIX OR GREYCSTORATION_INCLUDE_DIR)
   IF (WIN32)
      FIND_LIBRARY(OPENTHREADS_LIBRARY openthreads_static ${WIN32_OPENTHREADS_PATH})
      FIND_PATH(OPENTHREADS_INCLUDE_DIR OpenThreads/Thread ${WIN32_OPENTHREADS_PATH})
      INCLUDE_DIRECTORIES("${OPENTHREADS_INCLUDE_DIR}")
      ADD_DEFINITIONS(-DOT_LIBRARY_STATIC)
   ELSE (WIN32)
      FIND_PACKAGE(OpenThreads)
   ENDIF (WIN32)
   ADD_DEFINITIONS(-DUSEOPENTH)
   SET(THREAD_LIBRARY ${OPENTHREADS_LIBRARY})
ENDIF (UNIX OR GREYCSTORATION_INCLUDE_DIR)

# find CURL dependency
IF (WIN32)
   FIND_LIBRARY(CURL_LIBRARY curllib_static ${WIN32_CURL_PATH})
   FIND_PATH(CURL_INCLUDE_DIR curl/curl.h ${WIN32_CURL_PATH})
   INCLUDE_DIRECTORIES("${CURL_INCLUDE_DIR}")
   ADD_DEFINITIONS(-DCURL_STATICLIB)
ELSE (WIN32)
   FIND_PACKAGE(CURL)
ENDIF (WIN32)

# find Squish dependency
FIND_LIBRARY(SQUISH_LIBRARY squish ${SQUISH_PATH} ${WIN32_SQUISH_PATH})
FIND_PATH(SQUISH_INCLUDE_DIR squish.h ${SQUISH_PATH} ${WIN32_SQUISH_PATH})
INCLUDE_DIRECTORIES("${SQUISH_INCLUDE_DIR}")

# find GLUT dependency
IF (WIN32)
   FIND_LIBRARY(GLUT_LIBRARY freeglut_static ${WIN32_FREEGLUT_PATH})
   FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h ${WIN32_FREEGLUT_PATH})
   INCLUDE_DIRECTORIES("${GLUT_INCLUDE_DIR}")
   ADD_DEFINITIONS(-DFREEGLUT_STATIC)
ELSE (WIN32)
   FIND_PACKAGE(GLUT)
ENDIF (WIN32)

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

ADD_LIBRARY(Mini ${MINI_SRCS})
TARGET_LINK_LIBRARIES(Mini ${OPENGL_LIBRARIES})

ADD_LIBRARY(MiniSFX ${MINISFX_SRCS})
ADD_DEPENDENCIES(MiniSFX Mini)
TARGET_LINK_LIBRARIES(MiniSFX
   Mini
   ${THREAD_LIBRARY}
   ${CURL_LIBRARY}
   ${JPEG_LIBRARY}
   ${PNG_LIBRARY}
   ${SQUISH_LIBRARY}
   )

MACRO(MAKE_EXECUTABLE name)
   ADD_EXECUTABLE(${name} ${name}.cpp)
   TARGET_LINK_LIBRARIES(${name}
      Mini
      MiniSFX
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
   TARGETS Mini MiniSFX viewer
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   RUNTIME DESTINATION bin
   )

INSTALL(
   FILES ${MINI_HDRS}
   DESTINATION include/mini
   )
