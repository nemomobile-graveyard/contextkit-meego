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
#include "skyhook_interface.h"

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusPendingCallWatcher>
#include <MGConfItem>

const QString LocationProvider::satPositioningState("Location.SatPositioningState"); ///on, searching, off
const QString LocationProvider::coordinates("Location.Coordinates");
const QString LocationProvider::heading("Location.Heading");


IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new LocationProvider();
}

LocationProvider::LocationProvider() :
  gpsDevice(NULL)
{
	qDebug() << "LocationProvider " << "Initializing LocationProvider provider";
	QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);
}

LocationProvider::~LocationProvider()
{
    qDebug() << "LocationProvider::~LocationProvider()";
}

void LocationProvider::subscribe(QSet<QString> keys)
{
        qDebug() << "LocationProvider::subscribe(" << QStringList(keys.toList()).join(", ") << ")";

	if(!subscribedProps.count()) onFirstSubscriberAppeared();

	subscribedProps.unite(keys);

        QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void LocationProvider::unsubscribe(QSet<QString> keys)
{
        qDebug() << "LocationProvider::unsubscribe(" << QStringList(keys.toList()).join(", ") << ")";
	subscribedProps.subtract(keys);
	if(!subscribedProps.count()) onLastSubscriberDisappeared();
}

void LocationProvider::onFirstSubscriberAppeared()
{
	qDebug("first subscriber appeared!");
        qDebug() << "LocationProvider " << "First subscriber appeared, connecting to Skyhook";

        gpsDevice = new LocationSkyHook("com.skyhookwireless.wps.Daemon", "/com/skyhookwireless/wps/Daemon", QDBusConnection::sessionBus(), this);
        if(!gpsDevice->isValid())
        {
            qDebug("Location device interface not found");
            return;
        }

        connect(gpsDevice, SIGNAL(Location(double, double, double, double, double, double, double)),
                this, SLOT(locationChanged(double, double, double, double, double, double, double)));
        connect(gpsDevice, SIGNAL(Satellites(QList<int>, QList<int>, QList<int>, QList<int>, QList<bool>)),
                this, SLOT(satellitesChanged(QList<int>, QList<int>, QList<int>, QList<int>, QList<bool>)));

        QMetaObject::invokeMethod(this, "updateProperties", Qt::QueuedConnection);
}

void LocationProvider::onLastSubscriberDisappeared()
{
	qDebug() << "LocationProvider" << "Last subscriber gone, destroying LocationProvider connections";
        //TODO: disconnect from skyhook at this point
}

void LocationProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProps)
	{
		emit subscribeFinished(key);
	}
}

void LocationProvider::updateProperty(const QString& key, const QVariant& value)
{
    bool changed = true;
    if (Properties[key] == value) {
      changed = false;
    }

    Properties[key] = value;
    if (subscribedProps.contains(key) && changed) {
      emit valueChanged(key, value);
    }
}


void LocationProvider::locationChanged(double latitude, double longitude, double hpe, double altitude, double speed, double bearing, double timestamp)
{
    //FIXME: use fields correctly
    qDebug() << "LocationProvider:" << "New location values:";
    qDebug() << "\ttimestamp:" << timestamp << endl
             << "\tlatitude:" << latitude << endl
             << "\tlongitude:" << longitude << endl
             << "\taltitude:" << altitude << endl
             << "\thpe:" << hpe << endl
             << "\tspeed:" << speed << endl
             << "\tspeed:" << bearing;

    QList<QVariant> coords;
    coords.append(QVariant(latitude));
    coords.append(QVariant(longitude));
    coords.append(QVariant(altitude));
    updateProperty("Location.Coordinates", coords);
    updateProperty("Location.Heading", bearing);
}

void LocationProvider::satellitesChanged(QList<int> prns, QList<int> snrs, QList<int> elevations, QList<int> azimuths, QList<bool> inuse)
{
    //FIXME: use fields correctly
    qDebug() << "LocationProvider:" << "New satellite values:";

    bool satelliteOn = false;

  for(int satellite = 0; satellite < prns.count(); satellite++){
    qDebug() << "\tprns: " << prns.at(satellite) << endl
             << "\tsnrs: " << snrs.at(satellite) << endl
             << "\televations: " << elevations.at(satellite) << endl
             << "\tazimuths: " << azimuths.at(satellite) << endl
             << "\tinuse: " << inuse.at(satellite);
               if(inuse.at(satellite) == true)
                    satelliteOn = true;
  }

  QString state = (satelliteOn ? "on" : "off" ); //what is valid for searching?

    updateProperty("Location.SatPositioningState", state);
}
