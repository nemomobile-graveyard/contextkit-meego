TEMPLATE = app
TARGET = battery-test
QT += core
QT -= gui
SOURCES += main.cpp \
    battery.cpp

CONFIG += debug link_pkgconfig

PKGCONFIG += contextsubscriber-1.0

HEADERS += battery.h
