# cmake application build file
# (c) by Stefan Roettger

# expects ${APPNAME} to be set to the name of the application
# expects ${LIBMINI_PATH} to point to the libMini root directory (for non-standard installations)

PROJECT(${APPNAME})

# path to libMini
IF (NOT LIBMINI_PATH)
   SET(LIBMINI_PATH $ENV{LIBMINI_PATH})
   IF (NOT LIBMINI_PATH)
      FIND_PATH(LIBMINI_PATH mini.h ${CMAKE_CURRENT_SOURCE_DIR}/../mini ${CMAKE_CURRENT_SOURCE_DIR}/../../libmini/mini)
   ENDIF(NOT LIBMINI_PATH)
ENDIF(NOT LIBMINI_PATH)

INCLUDE(${LIBMINI_PATH}/libMini-config.cmake)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

MAKE_MINI_EXECUTABLE(${APPNAME})
