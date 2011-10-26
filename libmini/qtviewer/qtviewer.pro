QT += core gui opengl

TARGET = qtviewer
TEMPLATE = app

include(sources.pro)

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../..
INCLUDEPATH += $$PWD/../deps
INCLUDEPATH += $$PWD/../deps/squish
INCLUDEPATH += $$PWD/../deps/curl/include

win32:INCLUDEPATH += $$PWD/../WIN32
win32:INCLUDEPATH += $$PWD/../WIN32/pthreads-win32
win32:INCLUDEPATH += $$PWD/../WIN32/libcurl
win32:INCLUDEPATH += $$PWD/../WIN32/squish
win32:INCLUDEPATH += $$PWD/../WIN32/libjpeg
win32:INCLUDEPATH += $$PWD/../WIN32/libpng
win32:INCLUDEPATH += $$PWD/../WIN32/zlib

LIBS += -L$$PWD/..
LIBS += -L$$PWD/../mini
LIBS += -L$$PWD/../mini/debug
LIBS += -L$$PWD/../mini/release
LIBS += -L$$PWD/../deps
LIBS += -L$$PWD/../deps/squish
LIBS += -L$$PWD/../deps/curl/lib

win32:LIBS += -L$$PWD/../WIN32
win32:LIBS += -L$$PWD/../WIN32/pthreads-win32
win32:LIBS += -L$$PWD/../WIN32/libcurl
win32:LIBS += -L$$PWD/../WIN32/squish
win32:LIBS += -L$$PWD/../WIN32/libjpeg
win32:LIBS += -L$$PWD/../WIN32/libpng
win32:LIBS += -L$$PWD/../WIN32/zlib

unix:LIBS += -lMini -lMiniSFX
unix:LIBS += -lsquish -lcurl -ljpeg -lpng -lz

win32:LIBS += -llibMini -llibMiniSFX
win32:LIBS += -lsquish -lcurllib_static -llibjpeg -llibpng -lzlib -lpthread_static
win32:LIBS += -lws2_32 -lwinmm

RESOURCES += qtviewer.qrc

linux-g++:QMAKE_CXXFLAGS += -O2
linux-g++:QMAKE_CXXFLAGS_WARN_ON += -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:DEFINES += PTW32_STATIC_LIB
win32:DEFINES += CURL_STATICLIB
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305

mac:ICON = qtviewer.icns
win32:RC_FILE = ../mini/libMini.rc
