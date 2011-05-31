include(../common.pri)

system(qdbusxml2cpp -N -c Music -p music_interface.h:music_interface.cpp com.meego.app.Music.xml)

TEMPLATE = lib
SUBDIRS += tests
CONFIG += debug \
          link_pkgconfig \
          plugin
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0
TARGET = media
target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

SOURCES += \
mediaprovider.cpp \
music_interface.cpp

HEADERS += \
mediaprovider.h \
music_interface.h

OTHER_FILES += media.context \
    com.meego.app.Music.xml

configuration.files = media.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
