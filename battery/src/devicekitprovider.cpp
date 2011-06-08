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
#include <QDBusServiceWatcher>
#include <MGConfItem>

#define OnBattery "Battery.OnBattery"
#define ChargePercentage "Battery.ChargePercentage"
#define LowBattery "Battery.LowBattery"
#define TimeUntilLow "Battery.TimeUntilLow"
#define TimeUntilFull "Battery.TimeUntilFull"
#define IsCharging "Battery.IsCharging"
#define ChargeBars "Battery.ChargeBars"


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

	Q_UNUSED(powerdevices);//throw away the result

	if(!deviceKit.isValid())
	{
		qDebug()<<"devicekit (UPower) interface not found!";
		qDebug()<<"error message: "<<QDBusConnection::systemBus().lastError().message();

		QDBusServiceWatcher *watcher = new QDBusServiceWatcher("org.freedesktop.UPower",
															   QDBusConnection::systemBus(),
															   QDBusServiceWatcher::WatchForRegistration,this);

		connect(watcher,SIGNAL(serviceRegistered(QString)),this,SLOT(getBattery(QString)));

		return;
	}

	getBattery("");
}

void DeviceKitProvider::getBattery(QString)
{
	Power deviceKit("org.freedesktop.UPower",
			"/org/freedesktop/UPower",
			QDBusConnection::systemBus());

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
        Properties[ChargePercentage] = (int) batteryDevice->percentage();
	Properties[LowBattery] = batteryDevice->percentage() < 10;
	Properties[TimeUntilLow] = batteryDevice->timeToEmpty();
	Properties[TimeUntilFull] = batteryDevice->timeToFull();
	Properties[IsCharging] = batteryDevice->state() == 1 || batteryDevice->state() == 4;

        MGConfItem *numChargeBars = new MGConfItem("/gconf/meego/apps/contextkit/battery/chargebars");
        qDebug() << "DeviceKitPowerProvider" << "ChargeBars value is" << numChargeBars->value().toInt();

        if(numChargeBars->value().toInt() <= 0){
            qDebug() << "DeviceKitPowerProvider" << "invalid /gconf/meego/apps/contextkit/battery/chargebars key";
            numChargeBars->set(10); //set default to 10
        }

        QList<QVariant> bars;
        int maxBars = numChargeBars->value().toInt();


        if(batteryDevice->percentage() > 100 || batteryDevice->percentage() < 0) //If percentage is corrupted
            bars.append(50/maxBars);
        else
            bars.append((int)batteryDevice->percentage()/maxBars);

        bars.append(maxBars);

        Properties[ChargeBars] = QVariant(bars);

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


