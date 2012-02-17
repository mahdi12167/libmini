# libMini header and module list

SET(MINI_HDRS
   mini.h minibase.h
   minicore.h minicoreP.h
   miniOGL.h miniOGLP.h
   minivec.h minimtx.h
   minidyna.h minisort.h ministring.h minikeyval.h
   miniref.h mininode.h mininodes.h
   miniv3f.h miniv3d.h miniv4f.h miniv4d.h
   minimath.h minicomplex.h mininoise.h minimpfp.h
   minitime.h minirgb.h minicrs.h
   miniio.h minidir.h minidds.h
   ministub.h minitile.h miniload.h
   minicoord.h minicurve.h miniwarp.h minicam.h
   minilayer.h miniterrain.h miniearth.h
   minicache.h minishader.h
   miniray.h ministrip.h
   minipoint.h minitext.h minisky.h miniglobe.h
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
   minimath.cpp mininoise.cpp minimpfp.cpp
   minitime.cpp minirgb.cpp minicrs.cpp
   miniio.cpp minidir.cpp minidds.cpp
   ministub.cpp minitile.cpp miniload.cpp
   minicoord.cpp minicurve.cpp miniwarp.cpp minicam.cpp
   minilayer.cpp miniterrain.cpp miniearth.cpp
   minicache.cpp minishader.cpp
   miniray.cpp ministrip.cpp
   minipoint.cpp minitext.cpp minisky.cpp miniglobe.cpp
   minitree.cpp minibrick.cpp minilod.cpp
   minimesh.cpp minibspt.cpp miniproj.cpp
   mininodes.cpp
   pnmbase.cpp pnmsample.cpp
   database.cpp datafill.cpp
   datacloud.cpp datacache.cpp datagrid.cpp
   datacalc.cpp dataparse.cpp
   lunascan.cpp lunaparse.cpp lunacode.cpp
   )

SET(MINISFX_HDRS
   miniscene.h miniview.h
   threadbase.h curlbase.h
   jpegbase.h pngbase.h zlibbase.h squishbase.h greycbase.h
   dataconv.h miniimg.h
   minipano.h
   )

SET(MINISFX_SRCS
   miniscene.cpp miniview.cpp
   threadbase.cpp curlbase.cpp
   jpegbase.cpp pngbase.cpp zlibbase.cpp squishbase.cpp greycbase.cpp
   dataconv.cpp miniimg.cpp
   minipano.cpp
   )

IF (NOT WIN32)
   IF (MINI_BUILD_TYPE MATCHES RELEASE)
      SET_SOURCE_FILES_PROPERTIES(greycbase.cpp COMPILE_FLAGS -O) # -O3 takes too long
   ENDIF (MINI_BUILD_TYPE MATCHES RELEASE)
ENDIF (NOT WIN32)
