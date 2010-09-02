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

#include "devicekitprovider.h"
#include "power_interface.h"

#include <QDBusConnection>

#define OnBattery "Battery.OnBattery"
#define ChargePercentage "Battery.ChargePercentage"
#define LowBattery "Battery.LowBattery"
#define TimeUntilLow "Battery.TimeUntilLow"
#define TimeUntilFull "Battery.TimeUntilFull"
#define IsCharging "Battery.IsCharging"


//typedef OrgFreedesktopDeviceKitPowerInterface Power;

IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new DeviceKitProvider();
}

DeviceKitProvider::DeviceKitProvider()
	:batteryDevice(NULL)
{
	qDebug() << "DeviceKitPowerProvider " << "Initializing DeviceKit provider";
	QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);
}

DeviceKitProvider::~DeviceKitProvider()
{

}

void DeviceKitProvider::subscribe(QSet<QString> keys)
{
	qDebug() << "DeviceKitPowerProvider " << "subscribed to DeviceKit provider";

	if(!subscribedProps.count()) onFirstSubscriberAppeared();

	subscribedProps.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void DeviceKitProvider::unsubscribe(QSet<QString> keys)
{
	subscribedProps.subtract(keys);
	if(!subscribedProps.count()) onLastSubscriberDisappeared();
}

void DeviceKitProvider::onFirstSubscriberAppeared()
{
	qDebug("first subscriber appeared!");
	qDebug() << "DeviceKitPowerProvider " << "First subscriber appeared, connecting to DeviceKit";
	Power deviceKit("org.freedesktop.UPower",
			"/org/freedesktop/UPower",
			QDBusConnection::systemBus());
	///get my battery device:

	QList<QDBusObjectPath> powerdevices = deviceKit.EnumerateDevices();

	for(int i=0; i<powerdevices.size(); i++)
	{
		Battery * device = new Battery("org.freedesktop.UPower", powerdevices[i].path() ,QDBusConnection::systemBus());
		int devicetype = device->type();

		qDebug()<<"Power device type: "<< devicetype;

		if(devicetype == 2)
		{
			batteryDevice = device;
			connect(batteryDevice,SIGNAL(Changed()),this,SLOT(updateProperties()));

			break;
		}
	}
	if (!batteryDevice)
	{
		qDebug() << "DeviceKitPowerProvider" << " No valid battery device found";
	}

	QMetaObject::invokeMethod(this, "updateProperties", Qt::QueuedConnection);
}

void DeviceKitProvider::onLastSubscriberDisappeared()
{
	qDebug() << "DeviceKitPowerProvider" << "Last subscriber gone, destroying DeviceKit connections";
	delete batteryDevice;
	batteryDevice = NULL;
}

void DeviceKitProvider::updateProperties()
{
	if(!batteryDevice) return;

	Properties[OnBattery] = batteryDevice->state() == 2 || batteryDevice->state() == 3;
	Properties[ChargePercentage] = batteryDevice->percentage();
	Properties[LowBattery] = batteryDevice->percentage() < 10;
	Properties[TimeUntilLow] = batteryDevice->timeToEmpty();
	Properties[TimeUntilFull] = batteryDevice->timeToFull();
	Properties[IsCharging] = batteryDevice->state() == 1 || batteryDevice->state() == 4;

	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, Properties[key]);
	}
}

void DeviceKitProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProps)
	{
		emit subscribeFinished(key);
	}
}


