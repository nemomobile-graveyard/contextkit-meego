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

#ifndef DEVICEKITPROVIDER_H
#define DEVICEKITPROVIDER_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <iproviderplugin.h>
#include <contextproperty.h>
#include "device_interface.h"

using ContextSubscriber::IProviderPlugin;

typedef Device Battery;

extern "C"
{
    IProviderPlugin* pluginFactory(const QString& constructionString);
}

class DeviceKitProvider : public IProviderPlugin
{
    Q_OBJECT

public:
    DeviceKitProvider();
    virtual ~DeviceKitProvider();

    virtual void subscribe(QSet<QString> keys);
    virtual void unsubscribe(QSet<QString> keys);
    virtual void blockUntilReady() {}
    virtual void blockUntilSubscribed(const QString&) {}

private:
    QHash<QString,QVariant> Properties;
    QSet<QString> subscribedProps;
    Battery *batteryDevice; ///< The interface to the battery device

private slots:
    void updateProperties();
    void emitSubscribeFinished();
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();
	void getBattery(QString);

};


#endif // DEVICEKITPROVIDER_H
