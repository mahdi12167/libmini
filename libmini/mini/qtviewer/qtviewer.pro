QT += core gui opengl

TARGET = qtviewer
TEMPLATE = app


SOURCES += main.cpp \
           mainwindow.cpp \
           viewerwindow.cpp \
           renderer.cpp

HEADERS += mainwindow.h \
           mainconst.h \
           viewerwindow.h \
           viewerconst.h \
           renderer.h


INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../..
INCLUDEPATH += $$PWD/../deps $$PWD/../../deps
INCLUDEPATH += $$PWD/../deps/squish $$PWD/../../deps/squish
INCLUDEPATH += $$PWD/../deps/curl/include $$PWD/../../deps/curl/include

LIBS += -L$$PWD/..
LIBS += -L$$PWD/../mini
LIBS += -L$$PWD/../deps -L$$PWD/../../deps
LIBS += -L$$PWD/../deps/squish -L$$PWD/../../deps/squish
LIBS += -L$$PWD/../deps/curl/lib -L$$PWD/../../deps/curl/lib

LIBS += -lMini -lMiniSFX
LIBS += -lsquish -lcurl -ljpeg -lpng -lz

RESOURCES += qtviewer.qrc
