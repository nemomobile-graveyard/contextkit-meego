include(../../common.pri)

system(qdbusxml2cpp  -N -c CallVolume -i common.h -p callvolume_interface.h:callvolume_interface.cpp org.ofono.callvolume.xml)

system(qdbusxml2cpp  -N -c VoiceCallManager -i common.h -p callmanager_interface.h:callmanager_interface.cpp org.ofono.callmanager.xml)

system(qdbusxml2cpp -N -c Manager -i common.h -p manager_interface.h:manager_interface.cpp org.ofono.manager.xml)
system(qdbusxml2cpp -N -c VoiceCall -i common.h -p voicecall_interface.h:voicecall_interface.cpp org.ofono.voicecall.xml)


TEMPLATE = lib
CONFIG += debug \
    link_pkgconfig \
    plugin
QT += dbus
PKGCONFIG += contextprovider-1.0 contextsubscriber-1.0

TARGET = phone 
SOURCES += phoneprovider.cpp \
	manager_interface.cpp \
	callmanager.cpp\
	callitem.cpp \
	callvolume_interface.cpp\
	callmanager_interface.cpp\
	callproxy.cpp \
	voicecall_interface.cpp \
        common.cpp
	
	
HEADERS += common.h \
        phoneprovider.h \
	manager_interface.h\
	callmanager_interface.h\
	callvolume_interface.h\
	callmanager.h\
	callitem.h \
	callitemmodel.h\
	callproxy.h\
	voicecall_interface.h

target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

configuration.files = phone.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
