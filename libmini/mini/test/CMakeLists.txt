# cmake build file for libMini tests
# (c) by Stefan Roettger

PROJECT(${PROJECT_NAME}_tests)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

MAKE_MINISFX_EXECUTABLE(testmini)
MAKE_MINI_EXECUTABLE(testmpfp)
