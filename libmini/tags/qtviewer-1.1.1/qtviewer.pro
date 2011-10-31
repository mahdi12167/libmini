TARGET = qtviewer
TEMPLATE = app

include(sources.pro)

!include(../mini/sources.pro) {
   error("couldn't find libMini sources!")
}

DEPENDPATH += ../mini

QT += core gui opengl
CONFIG += debug_and_release build_all

MOC_DIR = mocs
OBJECTS_DIR = objs
UI_DIR = uics

INCLUDEPATH += .
INCLUDEPATH += ..
INCLUDEPATH += ../deps
INCLUDEPATH += ../deps/squish
INCLUDEPATH += ../deps/curl/include

win32:INCLUDEPATH += ../WIN32
win32:INCLUDEPATH += ../WIN32/pthreads-win32
win32:INCLUDEPATH += ../WIN32/libcurl
win32:INCLUDEPATH += ../WIN32/squish
win32:INCLUDEPATH += ../WIN32/libjpeg
win32:INCLUDEPATH += ../WIN32/libpng
win32:INCLUDEPATH += ../WIN32/zlib

LIBS += -L.
LIBS += -L..
LIBS += -L../deps
LIBS += -L../deps/squish
LIBS += -L../deps/curl/lib

win32:LIBS += -L../WIN32
win32:LIBS += -L../WIN32/pthreads-win32
win32:LIBS += -L../WIN32/libcurl
win32:LIBS += -L../WIN32/squish
win32:LIBS += -L../WIN32/libjpeg
win32:LIBS += -L../WIN32/libpng
win32:LIBS += -L../WIN32/zlib

unix:LIBS += -lsquish -lcurl -ljpeg -lpng -lz

win32:LIBS += -lsquish -lcurllib_static -llibjpeg -llibpng -lzlib -lpthread_static
win32:LIBS += -lws2_32 -lwinmm

RESOURCES += qtviewer.qrc

unix:QMAKE_CXXFLAGS += -O
unix:QMAKE_CXXFLAGS_WARN_ON += -w -Wall -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:DEFINES += PTW32_STATIC_LIB
win32:DEFINES += CURL_STATICLIB
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305

mac:ICON = qtviewer.icns
win32:RC_FILE = ../mini/libMini.rc
