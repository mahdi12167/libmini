# Defines
#  MINI_FOUND
#  MINISFX_FOUND
#  MINI_INCLUDE_DIR
#  MINI_LIBRARIES
#  MINISFX_LIBRARIES

FIND_PATH(MINI_INCLUDE_DIR mini/mini.h PATHS . .. ../deps DOC "Directory containing libMini headers")
FIND_LIBRARY(MINI_LIBRARY NAMES Mini libMini PATHS . .. ../mini ../deps/mini DOC "Path to libMini library")
FIND_LIBRARY(MINISFX_LIBRARY NAMES MiniSFX libMiniSFX PATHS . .. ../mini ../deps/mini DOC "Path to libMini SFX library")

SET(MINI_LIBRARIES ${MINI_LIBRARY})

IF (MINI_LIBRARY AND MINI_INCLUDE_DIR)
   SET(MINI_FOUND TRUE)
   MESSAGE(STATUS "Found libMini: ${MINI_LIBRARY}")
ENDIF (MINI_LIBRARY AND MINI_INCLUDE_DIR)

SET(MINISFX_LIBRARIES ${MINI_LIBRARY} ${MINISFX_LIBRARY})

IF (MINI_LIBRARY AND MINISFX_LIBRARY AND MINI_INCLUDE_DIR)
   SET(MINISFX_FOUND TRUE)
   MESSAGE(STATUS "Found libMiniSFX: ${MINISFX_LIBRARY}")
ENDIF (MINI_LIBRARY AND MINISFX_LIBRARY AND MINI_INCLUDE_DIR)
