# cmake build file for libMini tests
# (c) by Stefan Roettger

PROJECT(${PROJECT_NAME}_tools)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

MAKE_MINISFX_EXECUTABLE(test)
MAKE_MINI_EXECUTABLE(mpfptest)
