TARGET = Mini
TEMPLATE = lib

QT += opengl
QT -= gui

mac:QMAKESPEC = macx-g++

CONFIG += staticlib thread
CONFIG += debug_and_release build_all

CONFIG(debug, debug|release) {
   mac: TARGET = $$join(TARGET,,,_debug)
   win32: TARGET = $$join(TARGET,,d)
}

HEADERS += \
   mini.h minibase.h \
   minicore.h minicoreP.h \
   miniOGL.h miniOGLP.h \
   minivec.h minimtx.h \
   minidyna.h minisort.h ministring.h \
   miniref.h mininode.h mininodes.h \
   miniv3f.h miniv3d.h miniv4f.h miniv4d.h \
   minimath.h minicomplex.h minimpfp.h \
   minitime.h miniio.h minidir.h minirgb.h minicrs.h \
   ministub.h minitile.h miniload.h \
   minicoord.h miniwarp.h minicam.h minianim.h \
   minilayer.h miniterrain.h miniearth.h \
   minicache.h minishader.h \
   miniray.h ministrip.h \
   minipoint.h minitext.h minisky.h miniglobe.h \
   minitree.h minibrick.h minilod.h \
   minigeom.h minimesh.h minibspt.h miniproj.h \
   pnmbase.h pnmsample.h \
   database.h datafill.h \
   datacloud.h datacache.h datagrid.h \
   datacalc.h dataparse.h \
   lunascan.h lunaparse.h lunacode.h

SOURCES += \
   mini.cpp miniOGL.cpp \
   miniv3f.cpp miniv3d.cpp miniv4f.cpp miniv4d.cpp \
   minimath.cpp minimpfp.cpp \
   minitime.cpp miniio.cpp minidir.cpp \
   minirgb.cpp minicrs.cpp \
   ministub.cpp minitile.cpp miniload.cpp \
   minicoord.cpp miniwarp.cpp minicam.cpp minianim.cpp \
   minilayer.cpp miniterrain.cpp miniearth.cpp \
   minicache.cpp minishader.cpp \
   miniray.cpp ministrip.cpp \
   minipoint.cpp minitext.cpp minisky.cpp miniglobe.cpp \
   minitree.cpp minibrick.cpp minilod.cpp \
   minimesh.cpp minibspt.cpp miniproj.cpp \
   mininodes.cpp \
   pnmbase.cpp pnmsample.cpp \
   database.cpp datafill.cpp \
   datacloud.cpp datacache.cpp datagrid.cpp \
   datacalc.cpp dataparse.cpp \
   lunascan.cpp lunaparse.cpp lunacode.cpp

unix:QMAKE_CXXFLAGS += -O
unix:QMAKE_CXXFLAGS_WARN_ON += -w -Wall -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305

mac:ICON = libMini.icns
win32:RC_FILE = libMini.rc
