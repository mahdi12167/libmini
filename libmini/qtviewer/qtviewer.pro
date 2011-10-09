QT += core gui opengl

TARGET = qtviewer
TEMPLATE = app


SOURCES += main.cpp \
           mainwindow.cpp \
           viewerwindow.cpp \
           viewer.cpp \
           camera.cpp

HEADERS += mainwindow.h \
           mainconst.h \
           viewerwindow.h \
           viewerconst.h \
           viewer.h \
           camera.h


INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../..
INCLUDEPATH += $$PWD/../deps
INCLUDEPATH += $$PWD/../deps/squish
INCLUDEPATH += $$PWD/../deps/curl/include

LIBS += -L$$PWD/..
LIBS += -L$$PWD/../mini
LIBS += -L$$PWD/../deps
LIBS += -L$$PWD/../deps/squish
LIBS += -L$$PWD/../deps/curl/lib

LIBS += -lMini -lMiniSFX
LIBS += -lsquish -lcurl -ljpeg -lpng -lz

RESOURCES += qtviewer.qrc
