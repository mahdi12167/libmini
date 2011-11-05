TARGET = Mini
TEMPLATE = lib

include(sources.pro)

QT += opengl
QT -= gui

CONFIG += staticlib thread
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
   unix:TARGET = $$join(TARGET,,,_debug)
   win32:TARGET = $$join(TARGET,,d)
}

MOC_DIR = mocs
OBJECTS_DIR = objs
UI_DIR = uics

INCLUDEPATH += . ..

INCLUDEPATH += ../deps/squish
LIBS += -L../deps/squish

unix:QMAKE_CXXFLAGS += -O
unix:QMAKE_CXXFLAGS_WARN_ON += -w -Wall -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305

mac:ICON = libMini.icns
win32:RC_FILE = libMini.rc
