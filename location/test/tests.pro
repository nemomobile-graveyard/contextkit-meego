TEMPLATE = app
TARGET = location-test
QT += core
QT -= gui
SOURCES += main.cpp \
    location.cpp

CONFIG += debug link_pkgconfig

PKGCONFIG += contextsubscriber-1.0

HEADERS += location.h
