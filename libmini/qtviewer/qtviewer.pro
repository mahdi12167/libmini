QT += core gui opengl

TARGET = qtviewer
TEMPLATE = app

include(sources.pro)

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

linux-g++ {
QMAKE_CXXFLAGS_WARN_OFF += -Wno-parentheses
}

win32 {
DEFINES += _CRT_SECURE_NO_DEPRECATE
QMAKE_CXXFLAGS_WARN_OFF += /wd4244 /wd4305
}

mac {
ICON = qtviewer.icns
}

win32 {
RC_FILE = ../mini/libMini.rc
}
