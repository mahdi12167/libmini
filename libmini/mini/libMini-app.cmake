# cmake application build file
# (c) by Stefan Roettger

# expects ${APPNAME} to be set
# expects ${LIBMINI_PATH} to be set if libMini has not been installed in a standard directory

PROJECT(${APPNAME})

# path to libMini
IF (NOT LIBMINI_PATH)
   FIND_PATH(LIBMINI_PATH mini.h ${CMAKE_CURRENT_SOURCE_DIR}/../mini ${CMAKE_CURRENT_SOURCE_DIR}/../libmini/mini)
ENDIF (NOT LIBMINI_PATH)

INCLUDE(${LIBMINI_PATH}/libMini-config.cmake)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(APPLIB_SRCS
   ${APPNAME}.cpp
   )

ADD_LIBRARY(AppLib ${APPLIB_SRCS})
TARGET_LINK_LIBRARIES(AppLib
   ${MINI_LIBRARY}
   ${OPENGL_LIBRARIES}
   )

MACRO(MAKE_EXECUTABLE name)
   ADD_EXECUTABLE(${name} ${name}.cpp)
   TARGET_LINK_LIBRARIES(${name}
      AppLib
      ${MINI_LIBRARY}
      ${OPENGL_LIBRARIES}
      ${GLUT_LIBRARY}
      )
   IF (WIN32)
      TARGET_LINK_LIBRARIES(${name} ws2_32)
   ENDIF (WIN32)
ENDMACRO(MAKE_EXECUTABLE)

MAKE_EXECUTABLE(${APPNAME})
