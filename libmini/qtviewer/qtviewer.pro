TARGET = qtviewer
TEMPLATE = app

include(sources.pro)

!include(mini/sources.pro) {
   error("couldn't find libMini sources!")
}

!include(squish/sources.pro) {
   error("couldn't find libSquish sources!")
}

!include(grid/sources.pro) {
   error("couldn't find libGrid sources!")
}

DEPENDPATH += mini
DEPENDPATH += squish
DEPENDPATH += grid

QT += core gui opengl
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
   unix:TARGET = $$join(TARGET,,,_debug)
}

MOC_DIR = mocs
OBJECTS_DIR = objs
RCC_DIR = rccs
UI_DIR = uics

CONFIG(debug, debug|release) {
   unix:MOC_DIR = $$join(MOC_DIR,,,_debug)
   unix:OBJECTS_DIR = $$join(OBJECTS_DIR,,,_debug)
   unix:RCC_DIR = $$join(RCC_DIR,,,_debug)
   unix:UI_DIR = $$join(UI_DIR,,,_debug)
   win32:MOC_DIR = $$join(MOC_DIR,,,d)
   win32:OBJECTS_DIR = $$join(OBJECTS_DIR,,,d)
   win32:RCC_DIR = $$join(RCC_DIR,,,d)
   win32:UI_DIR = $$join(UI_DIR,,,d)
}

INCLUDEPATH += .
INCLUDEPATH += squish
INCLUDEPATH += ..
INCLUDEPATH += ../..
exists(../include): INCLUDEPATH += ../include
exists(../../include): INCLUDEPATH += ../../include
!exists(/usr/include/curl.h): INCLUDEPATH += ../../libcurl/include
!exists(/usr/include/jpeglib.h): INCLUDEPATH += ../../libjpeg/include
!exists(/usr/include/png.h): INCLUDEPATH += ../../libpng/include
!exists(/usr/include/zlib.h): INCLUDEPATH += ../../zlib/include
!exists(/usr/include/gdal.h): INCLUDEPATH += ../../gdal/include
!exists(/usr/include/iconv.h): INCLUDEPATH += ../../libiconv/include

win32:INCLUDEPATH += ../WIN32
win32:INCLUDEPATH += ../WIN32/pthreads-win32
win32:INCLUDEPATH += ../WIN32/libcurl
win32:INCLUDEPATH += ../WIN32/libjpeg
win32:INCLUDEPATH += ../WIN32/libpng
win32:INCLUDEPATH += ../WIN32/zlib
win32:INCLUDEPATH += ../../gdal

LIBS += -L.
LIBS += -L..
LIBS += -L../..
exists(../lib): LIBS += -L../lib
exists(../../lib): LIBS += -L../../lib
!exists(/usr/include/curl.h): LIBS += -L../../libcurl/lib
!exists(/usr/include/jpeglib.h): LIBS += -L../../libjpeg/lib
!exists(/usr/include/png.h): LIBS += -L../../libpng/lib
!exists(/usr/include/zlib.h): LIBS += -L../../zlib/lib
!exists(/usr/include/gdal.h): LIBS += -L../../gdal/lib
!exists(/usr/include/iconv.h): LIBS += -L../../libiconv/lib

win32:LIBS += -L../WIN32
win32:LIBS += -L../WIN32/pthreads-win32
win32:LIBS += -L../WIN32/libcurl
win32:LIBS += -L../WIN32/libjpeg
win32:LIBS += -L../WIN32/libpng
win32:LIBS += -L../WIN32/zlib
win32:LIBS += -L../../gdal

unix:LIBS += -lgdal -liconv
#unix:LIBS += -ljpeg -lpng
unix:LIBS += -lcurl
unix:LIBS += -lz

unix,!mac:LIBS += -lGLU

win32:LIBS += -lcurllib_static -llibjpeg -llibpng -lzlib -lpthread_static
win32:LIBS += -lgdal # -lgdal_i for dll stub
win32:LIBS += -lws2_32 -lwinmm

RESOURCES += qtviewer.qrc

DEFINES += WITH_SQUISH

CONFIG(debug, debug|release) {
   DEFINES += LIBMINI_DEBUG
}

unix:QMAKE_CXXFLAGS += -O
unix:QMAKE_CXXFLAGS_RELEASE += -O3
unix:QMAKE_CXXFLAGS_WARN_ON += -Wall -Wno-unused-parameter -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:DEFINES += PTW32_STATIC_LIB
win32:DEFINES += CURL_STATICLIB
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305 /wd4100

mac:ICON = qtviewer.icns
win32:RC_FILE = mini/libMini.rc
