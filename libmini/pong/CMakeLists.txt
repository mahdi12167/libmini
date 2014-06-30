# cmake build file

IF (WIN32)
   SET(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "" FORCE)
ENDIF (WIN32)

PROJECT(PingPong)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

OPTION(BUILD_WITH_QT5 "Use Qt5 instead of Qt4." OFF)

# settings

# non-standard path to Qt4 source installation
SET (QT4_DIR /usr/local/Trolltech/Qt-4.7.4)
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}; ${QT4_DIR};)
INCLUDE_DIRECTORIES(${QT4_DIR}/include ${QT5_DIR}/include/QtGui ${QT5_DIR}/QtCore)

# non-standard path to Qt5
IF (BUILD_WITH_QT5)
   SET (QT5_DIR /usr/local/Qt-5.3.0)
   SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}; ${QT5_DIR};)
   INCLUDE_DIRECTORIES(${QT5_DIR}/include ${QT5_DIR}/include/QtWidgets ${QT5_DIR}/include/QtCore)
ENDIF (BUILD_WITH_QT5)

# default Unix compiler definitions
IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -finline-functions -Wall -Wno-unused-parameter -Wno-parentheses")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)

# dependencies

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};
    /usr/local/libjpeg;
    /usr/local/libpng;
    /usr/local/zlib)

FIND_PACKAGE(JPEG)
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

FIND_PACKAGE(PNG)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

FIND_PACKAGE(ZLIB)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

# Qt dependency
IF (BUILD_WITH_QT5)
   SET(CMAKE_AUTOMOC ON)
   SET(CMAKE_INCLUDE_CURRENT_DIR ON)
   FIND_PACKAGE(Qt5Widgets REQUIRED)
   FIND_PACKAGE(Qt5Network REQUIRED)
   FIND_PACKAGE(Qt5Sql REQUIRED)
   ADD_DEFINITIONS(-DHAVE_QT5)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
ELSE (BUILD_WITH_QT5)
   FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtNetwork QtSql REQUIRED)
   INCLUDE(${QT_USE_FILE})
   ADD_DEFINITIONS(${QT_DEFINITIONS})
ENDIF (BUILD_WITH_QT5)

FIND_PACKAGE(Threads)

# header and module list

SET(LIB_HDRS
   # module headers
   sslsocket.h
   ssltransmission.h
   ssldatabase.h
   sslserver.h
   sslclient.h
   sslqueue.h
   serverui.h
   clientui.h
   )

SET(LIB_SRCS
   # module sources
   sslsocket.cpp
   ssltransmission.cpp
   ssldatabase.cpp
   sslserver.cpp
   sslclient.cpp
   sslqueue.cpp
   serverui.cpp
   clientui.cpp
   )

# moc

IF (NOT BUILD_WITH_QT5)
   QT4_WRAP_CPP(MOC_OUTFILES ${LIB_HDRS})
ENDIF (NOT BUILD_WITH_QT5)

# library

SET(LIB_NAME ${PROJECT_NAME})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(${LIB_NAME} ${LIB_SRCS} ${LIB_HDRS} ${MOC_OUTFILES})

# executable

SET(MACOSX_BUNDLE_ICON_FILE "pong.icns") # bundle icon
SET_SOURCE_FILES_PROPERTIES(
   ${MACOSX_BUNDLE_ICON_FILE}
   PROPERTIES
   MACOSX_PACKAGE_LOCATION Resources
   )

# build macro
MACRO(MAKE_EXECUTABLE name main)
   # compile and link executable
   ADD_EXECUTABLE(${name} MACOSX_BUNDLE ${main} ${MACOSX_BUNDLE_ICON_FILE})
   TARGET_LINK_LIBRARIES(${name}
      ${LIB_NAME}
      )
   IF (BUILD_WITH_QT5)
      TARGET_LINK_LIBRARIES(${name}
         Qt5::Widgets
         Qt5::Network
         Qt5::Sql
         )
   ELSE (BUILD_WITH_QT5)
      TARGET_LINK_LIBRARIES(${name}
         ${QT_LIBRARIES}
         )
   ENDIF (BUILD_WITH_QT5)
   TARGET_LINK_LIBRARIES(${name}
      ${JPEG_LIBRARIES}
      ${PNG_LIBRARIES}
      ${ZLIB_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      )
   IF (UNIX AND NOT APPLE)
      TARGET_LINK_LIBRARIES(${name} dl rt)
   ENDIF (UNIX AND NOT APPLE)

   # install target
   INSTALL(
      TARGETS ${name}
      RUNTIME DESTINATION bin
      BUNDLE DESTINATION /Applications
      )
ENDMACRO(MAKE_EXECUTABLE name main)

# build executables
MAKE_EXECUTABLE(pong pong.cpp)
MAKE_EXECUTABLE(ping ping.cpp)

# install .desktop file and icon for KDE4 (and Gnome)
IF (UNIX AND NOT APPLE)
   INSTALL(FILES Pong.desktop DESTINATION /usr/share/applications)
   INSTALL(FILES pong.png DESTINATION /usr/share/pingpong)
   INSTALL(FILES Ping.desktop DESTINATION /usr/share/applications)
   INSTALL(FILES ping.png DESTINATION /usr/share/pingpong)
ENDIF (UNIX AND NOT APPLE)

# install pong data
IF (UNIX)
   INSTALL(FILES cert.pem key.pem DESTINATION /usr/share/pingpong)
ENDIF (UNIX)
