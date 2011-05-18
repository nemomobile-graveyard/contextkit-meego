include(../common.pri)

TEMPLATE = lib
CONFIG += debug \
          link_pkgconfig \
          plugin
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0
TARGET = bluetooth
PKGCONFIG += bluetooth-qt
target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

SOURCES += bluetoothprovider.cpp
HEADERS += bluetoothprovider.h

OTHER_FILES += bluetooth.context

configuration.files = bluetooth.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration

