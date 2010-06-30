include(../../common.pri)

!exists(callvolume_interface.h):system(qdbusxml2cpp  -N -c CallVolume -p callvolume_interface.h:callvolume_interface.cpp org.ofono.callvolume.xml)

!exists(callmanager_interface.h):system(qdbusxml2cpp  -N -c VoiceCallManager -p callmanager_interface.h:callmanager_interface.cpp org.ofono.callmanager.xml)

!exists(manager_interface.h):system(qdbusxml2cpp -N -c Manager -p manager_interface.h:manager_interface.cpp org.ofono.manager.xml)
!exists(voicecall_interface.h):system(qdbusxml2cpp -N -c VoiceCall -p voicecall_interface.h:voicecall_interface.cpp org.ofono.voicecall.xml)


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
	voicecall_interface.cpp
	
	
HEADERS += phoneprovider.h \
	manager_interface.h\
	callmanager_interface.h\
	callvolume_interface.h\
	callmanager.h\
	callitem.h \
	callitemmodel.h\
	callproxy.h\
	voicecall_interface.h \
	common.h

target.path = $$CONTEXTKIT_SUBSCRIBER_PLUGIN_PATH

configuration.files = phone.context
configuration.path = $$CONTEXTKIT_PROVIDER_CONTEXT_PATH

INSTALLS += target configuration
