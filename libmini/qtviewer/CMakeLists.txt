# cmake build file

PROJECT(QTV)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

OPTION(BUILD_QTVIEWER_TEST "Build QTViewer with test scene." OFF)

# settings

# non-standard path to Qt4 source installation
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};
    /usr/local/Trolltech/Qt-4.7.4;
   )

# default Unix compiler definitions
IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -finline-functions -Wall -Wno-unused-parameter -Wno-parentheses")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)

# dependencies

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};
    /usr/local/squish;
    /usr/local/libcurl;
    /usr/local/gdal;
    /usr/local/libiconv;
    /usr/local/libjpeg;
    /usr/local/libpng;
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

FIND_PACKAGE(JPEG)
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

FIND_PACKAGE(PNG)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

FIND_PACKAGE(ZLIB)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

SET(QT_USE_QTOPENGL TRUE)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

FIND_PACKAGE(OpenGL)

FIND_PACKAGE(Threads)

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

IF (BUILD_QTVIEWER_TEST)
   ADD_DEFINITIONS(-DVIEWER_TEST)
ENDIF (BUILD_QTVIEWER_TEST)

ADD_EXECUTABLE(qtviewer main.cpp)
TARGET_LINK_LIBRARIES(qtviewer
   ${LIB_NAME}
   ${GRID_LIBRARIES}
   ${MINISFX_LIBRARIES}
   ${LIBSQUISH_LIBRARIES}
   ${CURL_LIBRARIES}
   ${GDAL_LIBRARIES}
   ${ICONV_LIBRARIES}
   ${JPEG_LIBRARIES}
   ${PNG_LIBRARIES}
   ${ZLIB_LIBRARIES}
   ${QT_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${CMAKE_THREAD_LIBS_INIT}
   )

# install target
INSTALL(
   TARGETS qtviewer
   RUNTIME DESTINATION bin
)

# install .desktop file and icon for KDE4 (and Gnome)
IF (UNIX AND NOT APPLE)
   INSTALL(FILES QTViewer.desktop DESTINATION /usr/share/applications)
   INSTALL(FILES qtviewer.png DESTINATION /usr/share/mini)
ENDIF (UNIX AND NOT APPLE)

# install mini data
IF (UNIX AND NOT APPLE)
   INSTALL(FILES data/EarthDay.db data/EarthNight.db DESTINATION /usr/share/mini)
ENDIF (UNIX AND NOT APPLE)
