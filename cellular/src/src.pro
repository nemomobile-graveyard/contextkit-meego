include(../../common.pri)

!exists(ofono_interface.h):system(qdbusxml2cpp -N -c NetworkProperties -p ofono_interface.h:ofono_interface.cpp org.ofono.networkregistration.xml)

!exists(manager_interface.h):system(qdbusxml2cpp -N -c Manager -p manager_interface.h:manager_interface.cpp org.ofono.manager.xml)


TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0

TARGET = cellular
SOURCES += cellularprovider.cpp ofono_interface.cpp manager_interface.cpp
HEADERS += cellularprovider.h ofono_interface.h manager_interface.h

target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

configuration.files = cellular.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
