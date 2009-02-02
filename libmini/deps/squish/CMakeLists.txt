# cmake build file for squish
# (c) by Stefan Roettger

PROJECT(squish)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(SQUISH_HDRS
   squish.h
   )

SET(SQUISH_SRCS
   alpha.cpp
   clusterfit.cpp
   colourblock.cpp
   colourfit.cpp
   colourset.cpp
   maths.cpp
   rangefit.cpp
   singlecolourfit.cpp
   squish.cpp
   )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(squish ${SQUISH_SRCS} ${SQUISH_HDRS})

INSTALL(
   TARGETS squish
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   )

INSTALL(
   FILES ${SQUISH_HDRS}
   DESTINATION include
   )
