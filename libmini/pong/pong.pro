TARGET = pong
TEMPLATE = app

QT += core gui network

HEADERS += sslsocket.h ssltransmission.h
SOURCES += pong.cpp sslsocket.cpp ssltransmission.cpp
