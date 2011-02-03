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

#ifndef PHONEPROVIDER_H
#define PHONEPROVIDER_H

#include <QVariant>
#include <QObject>
#include <iproviderplugin.h>
#include "callmanager.h"
#include <contextproperty.h>
#include "callvolume_interface.h"
#include "common.h"

using ContextSubscriber::IProviderPlugin;

extern "C"
{
	IProviderPlugin* pluginFactory(const QString& constructionString);
}

class PhoneProvider : public IProviderPlugin
{
	Q_OBJECT

public:
	PhoneProvider();
	virtual ~PhoneProvider();

	virtual void subscribe(QSet<QString> keys);
	virtual void unsubscribe(QSet<QString> keys);
	virtual void blockUntilReady() {}
	virtual void blockUntilSubscribed(const QString&) {}
	QList<QDBusObjectPath> providerProcessGetModems(QDBusPendingCallWatcher *);
private slots:
	void initProvider();
	void cleanProvider();
	void updateProperties();
	void updateCall();
	void updateProperty(const QString&, const QDBusVariant&);
	void emitSubscribeFinished();

private:
	QMap<QString,QVariant> props;
	QSet<QString> subscribedProps;
	CallVolume *volumeProps; 
	CallManager *callProps;
	uint m_currCalls; 
	uint m_currMpartyCalls; 
	CallItem *m_currActiveCall; 
};

#endif // PHONEPROVIDER_H
