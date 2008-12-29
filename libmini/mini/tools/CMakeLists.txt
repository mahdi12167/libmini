# cmake build file for libMini tools
# (c) by Stefan Roettger

PROJECT(${PROJECT_NAME}_tools)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

MAKE_EXECUTABLE(db2db)
MAKE_EXECUTABLE(eval)
MAKE_EXECUTABLE(mpfptest)
MAKE_EXECUTABLE(test)
