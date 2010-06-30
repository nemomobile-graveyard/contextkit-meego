TEMPLATE = app
TARGET = phone-test
QT += core
QT -= gui
SOURCES += main.cpp \
    phonetest.cpp

CONFIG += debug link_pkgconfig

PKGCONFIG += contextsubscriber-1.0

HEADERS += phonetest.h
