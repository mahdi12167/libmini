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

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../deps
INCLUDEPATH += $$PWD/../deps/squish
INCLUDEPATH += $$PWD/../deps/curl/include

LIBS += -L$$PWD/../mini
LIBS += -L$$PWD/../deps
LIBS += -L$$PWD/../deps/squish
LIBS += -L$$PWD/../deps/curl/lib

LIBS += -lMini -lMiniSFX
LIBS += -lsquish -lcurl -ljpeg -lpng -lz

RESOURCES += landscape.qrc
