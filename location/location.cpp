/*  -*- Mode: C++ -*-
 *
 * contextkit-meego
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "location.h"
#include "gypsy_interface.h"

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QXmlQuery>
#include <QDomDocument>

#define SatPositioningState "Location.SatPositioningState"
///on, searching, off

//typedef OrgFreedesktopDeviceKitPowerInterface Power;

IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new LocationProvider();
}

LocationProvider::LocationProvider()
{
	qDebug() << "LocationProvider " << "Initializing LocationProvider provider";

	QDBusInterface interface("org.freedesktop.Gypsy","/org/freedesktop/Gypsy",
				 "org.freedesktop.DBus.Introspectable", QDBusConnection::systemBus(), this);

	QDBusReply<QString> reply = interface.call("Introspect");

	QString xml = reply.value();

	QString devicePath = parseOutNode(xml);

	qDebug()<<"device path: "<<devicePath;

	if(devicePath.isEmpty())
	{
		//Q_ASSERT(0);
		gpsDevice = NULL;
	}

	else gpsDevice = new OrgFreedesktopGypsyDeviceInterface("org.freedesktop.Gypsy", devicePath,
							   QDBusConnection::systemBus(), this);

	connect(gpsDevice,SIGNAL(ConnectionStatusChanged(bool)),this,SLOT(connectionStatusChanged(bool)));
	connect(gpsDevice,SIGNAL(FixStatusChanged(int)),this, SLOT(fixStatusChanged(int)));

	QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);
}

LocationProvider::~LocationProvider()
{

}

void LocationProvider::subscribe(QSet<QString> keys)
{
	qDebug() << "LocationProvider " << "subscribed to LocationProvider provider";

	if(!subscribedProps.count()) onFirstSubscriberAppeared();

	subscribedProps.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void LocationProvider::unsubscribe(QSet<QString> keys)
{
	subscribedProps.subtract(keys);
	if(!subscribedProps.count()) onLastSubscriberDisappeared();
}

void LocationProvider::onFirstSubscriberAppeared()
{
	qDebug("first subscriber appeared!");
	qDebug() << "LocationProvider " << "First subscriber appeared, connecting to Gypsy";

	QMetaObject::invokeMethod(this, "updateProperties", Qt::QueuedConnection);
}

void LocationProvider::onLastSubscriberDisappeared()
{
	qDebug() << "LocationProvider" << "Last subscriber gone, destroying LocationProvider connections";

}

void LocationProvider::updateProperties()
{
	///TODO: update properties

	if(!gpsDevice || !gpsDevice->isValid())
	{
		qDebug("gpsDevice is not valid");
		return;
	}
	bool isConnected = gpsDevice->GetConnectionStatus();
	int fixStatus = gpsDevice->GetFixStatus();

	qDebug()<<" connected? "<<isConnected<<" fix status: "<<fixStatus;

	if(!isConnected)
		Properties[SatPositioningState] = "off";
	else
		Properties[SatPositioningState] = fixStatus == 1 ? "searching":"on";

	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, Properties[key]);
	}
}

void LocationProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProps)
	{
		emit subscribeFinished(key);
	}
}

QString LocationProvider::parseOutNode(QString xml)
{
	QXmlQuery query;
	query.setFocus(xml);
	query.setQuery("/node/node");

	Q_ASSERT(query.isValid());

	query.evaluateTo(&xml);

	QDomDocument doc;
	doc.setContent(xml);

	QDomNodeList nodes = doc.elementsByTagName("node");

	if(!nodes.size()) return "";

	QDomNode node = nodes.at(0);

	return "/org/freedesktop/Gypsy/"+node.toElement().attribute("name");
}

void LocationProvider::fixStatusChanged(int)
{
	updateProperties();
}

void LocationProvider::connectionStatusChanged(bool)
{
	updateProperties();
}
