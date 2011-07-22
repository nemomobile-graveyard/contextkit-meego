include(../common.pri)
system(qdbusxml2cpp -N -c LocationSkyHook -p skyhook_interface.h:skyhook_interface.cpp com.skyhookwireless.wps.Daemon.xml)
TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus xmlpatterns
PKGCONFIG += contextprovider-1.0 \
    contextsubscriber-1.0 \
    mlite
TARGET = location-skyhook
SOURCES += \
location.cpp \
skyhook_interface.cpp
HEADERS += \
location.h \
skyhook_interface.h
target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH
configuration.files = location-skyhook.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH
INSTALLS += target \
    configuration
OTHER_FILES += \
    com.skyhookwireless.wps.Daemon.xml
