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

#ifndef LOCATIONPROVIDER_H
#define LOCATIONPROVIDER_H

#include <QObject>
#include <iproviderplugin.h>
#include <contextproperty.h>
#include "skyhook_interface.h"

using ContextSubscriber::IProviderPlugin;

extern "C"
{
    IProviderPlugin* pluginFactory(const QString& constructionString);
}

class LocationProvider : public IProviderPlugin
{
    Q_OBJECT

public:
    LocationProvider();
    virtual ~LocationProvider();

    virtual void subscribe(QSet<QString> keys);
    virtual void unsubscribe(QSet<QString> keys);
    virtual void blockUntilReady() {}
    virtual void blockUntilSubscribed(const QString&) {}

private:
    static const QString skyhookService;
    static const QString satPositioningState;
    static const QString coordinates;
    static const QString heading;

    QHash<QString,QVariant> Properties;
    QSet<QString> subscribedProps;
    LocationSkyHook *gpsDevice;
    
    void updateProperty(const QString& key, const QVariant& value);
    void getCoordinates();
    void getHeading();

private slots:
    void updateProperties();
    void emitSubscribeFinished();
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();
    void locationChanged(double latitude, double longitude, double hpe, double altitude, double speed, double bearing, double timestamp);
    void satellitesChanged(QList<int> prns, QList<int> snrs, QList<int> elevations, QList<int> azimuths, QList<bool> inuse);

};


#endif // LOCATIONPROVIDER_H
