include(../common.pri)
!exists(gypsy_interface.h):system(qdbusxml2cpp -N -p gypsy_interface.h:gypsy_interface.cpp -i gypsytypes.h gypsy-client-full.xml)
TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus xmlpatterns
PKGCONFIG += contextprovider-1.0 \
    contextsubscriber-1.0 \
    mlite
TARGET = location
SOURCES += location.cpp \
    gypsy_interface.cpp
HEADERS += location.h \
    gypsy_interface.h \
    gypsytypes.h
target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH
configuration.files = location.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH
INSTALLS += target \
    configuration
