# cmake build file for libMini plotter demos
# (c) by Stefan Roettger

PROJECT(${PROJECT_NAME}_demos)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

MAKE_PLOTTER_EXECUTABLE(spiral)
MAKE_PLOTTER_EXECUTABLE(scroll)
MAKE_PLOTTER_EXECUTABLE(bounce)

MAKE_PLOTTER_EXECUTABLE(koch)
MAKE_PLOTTER_EXECUTABLE(fern)
MAKE_PLOTTER_EXECUTABLE(julia)

MAKE_PLOTTER_EXECUTABLE(kdtree)

MAKE_PLOTTER_EXECUTABLE(automaton)
