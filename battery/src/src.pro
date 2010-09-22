include(../../common.pri)

system(qdbusxml2cpp -N -c Power -p power_interface.h:power_interface.cpp DeviceKit.Power.xml)
system(qdbusxml2cpp -N -c Device -p device_interface.h:device_interface.cpp DeviceKit.Power.Device.xml)

TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0

TARGET = battery
SOURCES += devicekitprovider.cpp device_interface.cpp power_interface.cpp
HEADERS += devicekitprovider.h device_interface.h power_interface.h

target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

configuration.files = battery.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
