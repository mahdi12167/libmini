# Defines
#  GRID_FOUND
#  GRID_INCLUDE_DIR
#  GRID_LIBRARIES

FIND_PATH(GRID_INCLUDE_DIR grid/grid.h PATHS . .. DOC "Directory containing libGrid headers")
FIND_LIBRARY(GRID_LIBRARY NAMES Grid libGrid PATHS . grid .. ../grid PATH_SUFFIXES lib lib64 release minsizerel relwithdebinfo DOC "Path to libGrid library")

SET(GRID_LIBRARIES ${GRID_LIBRARY})

IF (GRID_LIBRARY AND GRID_INCLUDE_DIR)
   SET(GRID_FOUND TRUE)
   MESSAGE(STATUS "Found libGrid: ${GRID_LIBRARY}")
ENDIF (GRID_LIBRARY AND GRID_INCLUDE_DIR)
