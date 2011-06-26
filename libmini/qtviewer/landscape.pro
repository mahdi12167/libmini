QT += core gui opengl

TARGET = landscape
TEMPLATE = app


SOURCES += main.cpp \
           mainwindow.cpp \
           viewerwindow.cpp \
           renderer.cpp \
           renderer_control.cpp \
           renderer_overlay.cpp

HEADERS += mainwindow.h \
           viewerwindow.h \
           renderer.h \
           viewerconst.h

INCLUDEPATH = $$PWD/.. $$PWD/../deps

LIBS += -L$$PWD/../mini -lMini
LIBS += -L$$PWD/../mini -lMiniSFX
LIBS += -L$$PWD/../libs/libcurl/lib -lcurl
LIBS += -L$$PWD/../libs/squish -lsquish
LIBS += -L$$PWD/../libs/libjpeg -ljpeg
LIBS += -L$$PWD/../libs/libpng -lpng
LIBS += -L$$PWD/../libs/zlib -lz

RESOURCES += landscape.qrc
