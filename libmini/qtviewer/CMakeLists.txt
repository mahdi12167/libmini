# cmake build file

PROJECT(QTV)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

# dependencies

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};
    /usr/local/squish;
    /usr/local/libcurl;
    /usr/local/gdal;
    /usr/local/libiconv;
    /usr/local/zlib)

FIND_PACKAGE(GRID)
INCLUDE_DIRECTORIES(${GRID_INCLUDE_DIR})

FIND_PACKAGE(MINI)
INCLUDE_DIRECTORIES(${MINI_INCLUDE_DIR})

FIND_PACKAGE(libSquish)
INCLUDE_DIRECTORIES(${LIBSQUISH_INCLUDE_DIR})

FIND_PACKAGE(CURL)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})

FIND_PACKAGE(GDAL)
INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

FIND_PACKAGE(ICONV) # for gdal 1.9+
INCLUDE_DIRECTORIES(${ICONV_INCLUDE_DIR})

FIND_PACKAGE(ZLIB)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

SET(QT_USE_QTOPENGL TRUE)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

FIND_PACKAGE(OpenGL)

# header and module list

SET(LIB_HDRS
   # module headers
   mainwindow.h
   viewerwindow.h
   renderer.h
   viewer.h
   camera.h
   object.h
   objectlist.h
   objects.h
   worker.h
   nodes.h
   )

SET(LIB_SRCS
   # module sources
   mainwindow.cpp
   viewerwindow.cpp
   renderer.cpp
   viewer.cpp
   camera.cpp
   object.cpp
   objectlist.cpp
   objects.cpp
   worker.cpp
   nodes.cpp
   )

# moc

QT4_WRAP_CPP(MOC_OUTFILES ${LIB_HDRS})

# library

SET(LIB_NAME ${PROJECT_NAME})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(${LIB_NAME} ${LIB_SRCS} ${LIB_HDRS} ${MOC_OUTFILES})

# executable

ADD_EXECUTABLE(qtviewer main.cpp)
TARGET_LINK_LIBRARIES(qtviewer
   ${LIB_NAME}
   ${GRID_LIBRARIES}
   ${MINISFX_LIBRARIES}
   ${LIBSQUISH_LIBRARIES}
   ${CURL_LIBRARIES}
   ${GDAL_LIBRARIES}
   ${ICONV_LIBRARIES}
   ${ZLIB_LIBRARIES}
   ${QT_LIBRARIES}
   ${OPENGL_LIBRARIES}
   )
