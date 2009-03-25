# libMini header and module list

SET(MINI_HDRS
   mini.h miniOGL.h
   minibase.h minivec.h minimtx.h minidyna.h minisort.h
   miniv3f.h miniv3d.h miniv4f.h miniv4d.h
   minimath.h minimpfp.h minitime.h miniio.h minihsv.h minicrs.h
   ministub.h minitile.h miniload.h
   minicoord.h miniwarp.h
   minilayer.h miniterrain.h miniearth.h
   minicache.h minishader.h miniray.h
   ministrip.h minipoint.h minitext.h minisky.h miniglobe.h
   minitree.h minibrick.h minilod.h
   minigeom.h minimesh.h minibspt.h miniproj.h
   pnmbase.h pnmsample.h
   database.h datafill.h
   datacloud.h datacache.h datagrid.h
   datacalc.h dataparse.h
   lunascan.h lunaparse.h lunacode.h
   )

SET(MINI_SRCS
   mini.cpp miniOGL.cpp
   miniv3f.cpp miniv3d.cpp miniv4f.cpp miniv4d.cpp
   minimath.cpp minimpfp.cpp minitime.cpp miniio.cpp minihsv.cpp minicrs.cpp
   ministub.cpp minitile.cpp miniload.cpp
   minicoord.cpp miniwarp.cpp
   minilayer.cpp miniterrain.cpp miniearth.cpp
   minicache.cpp minishader.cpp miniray.cpp
   ministrip.cpp minipoint.cpp minitext.cpp minisky.cpp miniglobe.cpp
   minitree.cpp minibrick.cpp minilod.cpp
   minigeom.cpp minimesh.cpp minibspt.cpp miniproj.cpp
   pnmbase.cpp pnmsample.cpp
   database.cpp datafill.cpp
   datacloud.cpp datacache.cpp datagrid.cpp
   datacalc.cpp dataparse.cpp
   lunascan.cpp lunaparse.cpp lunacode.cpp
   )

SET(MINISFX_HDRS
   viewerbase.h
   threadbase.h curlbase.h
   jpegbase.h pngbase.h zlibbase.h squishbase.h greycbase.h
   convbase.h imgbase.h
   panorndr.h
   )

SET(MINISFX_SRCS
   viewerbase.cpp
   threadbase.cpp curlbase.cpp
   jpegbase.cpp pngbase.cpp zlibbase.cpp squishbase.cpp greycbase.cpp
   convbase.cpp imgbase.cpp
   panorndr.cpp
   )

IF (MINI_BUILD_TYPE MATCHES RELEASE)
   SET_SOURCE_FILES_PROPERTIES(greycbase.cpp COMPILE_FLAGS -O) # -O3 takes too long
ENDIF (MINI_BUILD_TYPE MATCHES RELEASE)
