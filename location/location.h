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

private:
    QHash<QString,QVariant> Properties;
    QSet<QString> subscribedProps;
    OrgFreedesktopGypsyDeviceInterface *gpsDevice;
    
    QString parseOutNode(QString xml);

private slots:
    void updateProperties();
    void emitSubscribeFinished();
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();

    void fixStatusChanged(int);
    void connectionStatusChanged(bool);
};


#endif // LOCATIONPROVIDER_H
