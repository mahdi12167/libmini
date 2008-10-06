PROJECT(squish)

cmake_minimum_required(VERSION 2.4)

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

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

ADD_LIBRARY(squish ${SQUISH_SRCS})

INSTALL(
   TARGETS squish
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   )

INSTALL(
   FILES ${SQUISH_HDRS}
   DESTINATION include
   )
