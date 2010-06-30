TEMPLATE = app
TARGET = cellular-test
QT += core
QT -= gui
SOURCES += main.cpp \
    cellulartest.cpp

CONFIG += debug link_pkgconfig

PKGCONFIG += contextsubscriber-1.0

HEADERS += cellulartest.h
