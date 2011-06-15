#-------------------------------------------------
#
# Project created by QtCreator 2011-01-27T00:13:40
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = landscape
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    viewerwindow.cpp \
    renderer.cpp \
    renderer_control.cpp \
    renderer_overlay.cpp

HEADERS  += mainwindow.h \
    viewerwindow.h \
    renderer.h \
    viewerconst.h

INCLUDEPATH = $$PWD/../libs $$PWD/../libs/libcurl/include

LIBS += -L$$PWD/../libs/mini -lMini
LIBS += -L$$PWD/../libs/mini -lMiniSFX
LIBS += -L$$PWD/../libs/libcurl/lib -lcurl
LIBS += -L$$PWD/../libs/squish -lsquish
LIBS += -L$$PWD/../libs/libjpeg -ljpeg
LIBS += -L$$PWD/../libs/libpng -lpng
LIBS += -L$$PWD/../libs/zlib -lz

RESOURCES += \
    landscape.qrc
