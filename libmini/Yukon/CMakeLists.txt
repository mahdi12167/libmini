# cmake application build file
# (c) by Stefan Roettger

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(APPNAME Yukon)

# path to libMini
IF (NOT LIBMINI_PATH)
   FIND_PATH(LIBMINI_PATH mini.h ${CMAKE_CURRENT_SOURCE_DIR}/../mini ${CMAKE_CURRENT_SOURCE_DIR}/../libmini/mini)
ENDIF (NOT LIBMINI_PATH)

INCLUDE(${LIBMINI_PATH}/libMini-config.cmake)
INCLUDE(${LIBMINI_PATH}/libMini-app.cmake)
