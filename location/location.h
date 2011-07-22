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

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <iproviderplugin.h>
#include <contextproperty.h>
#include "gypsy_interface.h"

class QDBusPendingCallWatcher;

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
    static const QString gypsyService;
    static const QString satPositioningState;
    static const QString coordinates;
    static const QString heading;

    QHash<QString,QVariant> Properties;
    QSet<QString> subscribedProps;
    OrgFreedesktopGypsyDeviceInterface *gpsDevice;
    OrgFreedesktopGypsyPositionInterface *position;
    OrgFreedesktopGypsyCourseInterface *course;
    
    void updateProperty(const QString& key, const QVariant& value);
    void getCoordinates();
    void getHeading();

private slots:
    void updateProperties();
    void emitSubscribeFinished();
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();

    void fixStatusChanged(int);
    void positionChanged(int fields, int timestamp, double latitude, double longitude, double altitude);
  void courseChanged(int fields, int timestamp, double speed, double direction, double climb);
    void connectionStatusChanged(bool);
    void getPositionFinished(QDBusPendingCallWatcher* watcher);
    void getCourseFinished(QDBusPendingCallWatcher* watcher);
};


#endif // LOCATIONPROVIDER_H
