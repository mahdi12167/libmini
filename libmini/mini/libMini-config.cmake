# path to libMini
IF (NOT LIBMINI_PATH)
   SET(LIBMINI_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF (NOT LIBMINI_PATH)

# Unix compiler definitions
IF (UNIX)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
ENDIF (UNIX)

# Windows compiler definitions
IF (WIN32)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305")
ENDIF (WIN32)

# check environment variable for third party directory
IF (NOT LIBMINI_THIRDPARTY_DIR)
   SET(LIBMINI_THIRDPARTY_DIR "$ENV{LIBMINI_THIRDPARTY_DIR}")
ENDIF(NOT LIBMINI_THIRDPARTY_DIR)

# paths to optional dependencies
SET(OPENTHREADS_PATH ${LIBMINI_PATH}/../deps/openthreads ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/openthreads)
SET(CURL_PATH ${LIBMINI_PATH}/../deps/libcurl ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libcurl)
SET(JPEG_PATH ${LIBMINI_PATH}/../deps/libjpeg ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libjpeg)
SET(PNG_PATH ${LIBMINI_PATH}/../deps/libpng ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libpng)
SET(ZLIB_PATH ${LIBMINI_PATH}/../deps/zlib ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/zlib)
SET(SQUISH_PATH ${LIBMINI_PATH}/../deps/squish ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/squish)
SET(GREYCSTORATION_PATH ${LIBMINI_PATH}/../deps/greycstoration ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/greycstoration)

# paths to WIN32 dependencies
SET(WIN32_PTHREAD_PATH ${LIBMINI_PATH}/../WIN32/pthreads-win32 ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/pthreads-win32)
SET(WIN32_OPENTHREADS_PATH ${LIBMINI_PATH}/../WIN32/openthreads ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/openthreads)
SET(WIN32_CURL_PATH ${LIBMINI_PATH}/../WIN32/libcurl ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libcurl)
SET(WIN32_JPEG_PATH ${LIBMINI_PATH}/../WIN32/libjpeg ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libjpeg)
SET(WIN32_PNG_PATH ${LIBMINI_PATH}/../WIN32/libpng ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/libpng)
SET(WIN32_ZLIB_PATH ${LIBMINI_PATH}/../WIN32/zlib ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/zlib)
SET(WIN32_SQUISH_PATH ${LIBMINI_PATH}/../WIN32/squish ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/squish)
SET(WIN32_FREEGLUT_PATH ${LIBMINI_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../WIN32/freeglut ${LIBMINI_THIRDPARTY_DIR} ${LIBMINI_THIRDPARTY_DIR}/freeglut)

# find OpenGL dependency
FIND_PACKAGE(OpenGL)

MACRO(FIND_MYLIBRARY name file paths)
   IF (NOT ${name})
      FIND_LIBRARY(${name} ${file} PATHS ${paths} PATH_SUFFIXES lib)
   ENDIF (NOT ${name})
ENDMACRO(FIND_MYLIBRARY)

MACRO(FIND_MYPATH name file paths)
   IF (NOT ${name})
      FIND_PATH(${name} ${file} PATHS ${paths} PATH_SUFFIXES include)
   ENDIF (NOT ${name})
ENDMACRO(FIND_MYPATH)

# find GREYCstoration dependency
FIND_MYPATH(GREYCSTORATION_INCLUDE_DIR CImg.h "${GREYCSTORATION_PATH}")
IF (GREYCSTORATION_INCLUDE_DIR)
   INCLUDE_DIRECTORIES(${GREYCSTORATION_INCLUDE_DIR})
   ADD_DEFINITIONS(-DUSEGREYC)
ENDIF (GREYCSTORATION_INCLUDE_DIR)

# find POSIX threads dependency
IF (UNIX OR GREYCSTORATION_INCLUDE_DIR)
   IF (WIN32)
      FIND_MYLIBRARY(PTHREAD_LIBRARY pthread_static "${WIN32_PTHREAD_PATH}")
      FIND_MYPATH(PTHREAD_INCLUDE_DIR pthread.h "${WIN32_PTHREAD_PATH}")
   ELSE (WIN32)
      FIND_PACKAGE(Threads)
   ENDIF (WIN32)
   INCLUDE_DIRECTORIES(${PTHREAD_INCLUDE_DIR})
   IF (WIN32)
      ADD_DEFINITIONS(-DPTW32_STATIC_LIB)
   ENDIF (WIN32)
   SET(THREAD_LIBRARY ${PTHREAD_LIBRARY})
ELSE (UNIX OR GREYCSTORATION_INCLUDE_DIR)
   FIND_MYLIBRARY(OPENTHREADS_LIBRARY OpenThreads "${OPENTHREADS_PATH}")
   FIND_MYLIBRARY(OPENTHREADS_LIBRARY openthreads_static "${WIN32_OPENTHREADS_PATH}")
   FIND_MYPATH(OPENTHREADS_INCLUDE_DIR OpenThreads/Thread "${OPENTHREADS_PATH} ${WIN32_OPENTHREADS_PATH}")
   INCLUDE_DIRECTORIES(${OPENTHREADS_INCLUDE_DIR})
   IF (WIN32)
      ADD_DEFINITIONS(-DOT_LIBRARY_STATIC)
   ENDIF (WIN32)
   ADD_DEFINITIONS(-DUSEOPENTH)
   SET(THREAD_LIBRARY ${OPENTHREADS_LIBRARY})
ENDIF (UNIX OR GREYCSTORATION_INCLUDE_DIR)

# find CURL dependency
FIND_MYLIBRARY(CURL_LIBRARY curl "${CURL_PATH}")
FIND_MYLIBRARY(CURL_LIBRARY curllib_static "${WIN32_CURL_PATH}")
FIND_MYPATH(CURL_INCLUDE_DIR curl/curl.h "${CURL_PATH} ${WIN32_CURL_PATH}")
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
IF (WIN32)
   ADD_DEFINITIONS(-DCURL_STATICLIB)
ENDIF (WIN32)

# find JPEG dependency
FIND_MYLIBRARY(JPEG_LIBRARY jpeg "${JPEG_PATH}")
FIND_MYLIBRARY(JPEG_LIBRARY libjpeg "${WIN32_JPEG_PATH}")
FIND_MYPATH(JPEG_INCLUDE_DIR jpeglib.h "${JPEG_PATH} ${WIN32_JPEG_PATH}")
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

# find PNG dependency
FIND_MYLIBRARY(PNG_LIBRARY png "${PNG_PATH}")
FIND_MYLIBRARY(PNG_LIBRARY libpng "${WIN32_PNG_PATH}")
FIND_MYPATH(PNG_INCLUDE_DIR png.h "${PNG_PATH} ${WIN32_PNG_PATH}")
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

# find ZLIB dependency
FIND_MYLIBRARY(ZLIB_LIBRARY z "${ZLIB_PATH}")
FIND_MYLIBRARY(ZLIB_LIBRARY zlib "${WIN32_ZLIB_PATH}")
FIND_MYPATH(ZLIB_INCLUDE_DIR zlib.h "${ZLIB_PATH} ${WIN32_ZLIB_PATH}")
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

# find Squish dependency
FIND_MYLIBRARY(SQUISH_LIBRARY squish "${SQUISH_PATH} ${WIN32_SQUISH_PATH}")
FIND_MYPATH(SQUISH_INCLUDE_DIR squish.h "${SQUISH_PATH} ${WIN32_SQUISH_PATH}")
INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_DIR})

# find GLUT dependency
IF (WIN32)
   FIND_MYLIBRARY(GLUT_LIBRARY freeglut_static "${WIN32_FREEGLUT_PATH}")
   FIND_MYPATH(GLUT_INCLUDE_DIR GL/glut.h "${WIN32_FREEGLUT_PATH}")
ELSE (WIN32)
   FIND_PACKAGE(GLUT)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
IF (WIN32)
   ADD_DEFINITIONS(-DFREEGLUT_STATIC)
ENDIF (WIN32)

# find libMini dependency
FIND_MYLIBRARY(MINI_LIBRARY Mini ${LIBMINI_PATH})
FIND_MYPATH(MINI_INCLUDE_DIR mini.h ${LIBMINI_PATH})
INCLUDE_DIRECTORIES(${MINI_INCLUDE_DIR}/..)

# find libMiniSFX dependency
FIND_MYLIBRARY(MINISFX_LIBRARY MiniSFX ${LIBMINI_PATH})
FIND_MYPATH(MINISFX_INCLUDE_DIR viewerbase.h ${LIBMINI_PATH})
INCLUDE_DIRECTORIES(${MINISFX_INCLUDE_DIR}/..)

# include libMini source list
INCLUDE(${LIBMINI_PATH}/libMini.cmake)
