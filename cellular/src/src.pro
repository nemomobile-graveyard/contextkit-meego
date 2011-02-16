include(../../common.pri)

system(qdbusxml2cpp -N -c NetworkProperties -i common.h -p ofono_interface.h:ofono_interface.cpp org.ofono.networkregistration.xml)
system(qdbusxml2cpp -N -c SimProperties -i common.h -p sim_interface.h:sim_interface.cpp org.ofono.simmanager.xml)
system(qdbusxml2cpp -N -c Manager -i common.h -p manager_interface.h:manager_interface.cpp org.ofono.manager.xml)


TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0

TARGET = cellular
SOURCES += cellularprovider.cpp common.cpp ofono_interface.cpp manager_interface.cpp sim_interface.cpp
HEADERS += common.h cellularprovider.h ofono_interface.h manager_interface.h sim_interface.h

target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

configuration.files = cellular.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
