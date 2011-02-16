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

#ifndef CELLULARPROVIDER_H
#define CELLULARPROVIDER_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <iproviderplugin.h>
#include <contextproperty.h>
#include "ofono_interface.h"
#include "manager_interface.h"
#include "sim_interface.h"
#include "common.h"

using ContextSubscriber::IProviderPlugin;

extern "C"
{
	IProviderPlugin* pluginFactory(const QString& constructionString);
}

class CellularProvider : public IProviderPlugin
{
	Q_OBJECT

public:
	CellularProvider();
	virtual ~CellularProvider();

	virtual void subscribe(QSet<QString> keys);
	virtual void unsubscribe(QSet<QString> keys);
	virtual void blockUntilReady() {}
	virtual void blockUntilSubscribed(const QString&) {}
	QList<QDBusObjectPath> providerProcessGetModems(QDBusPendingCallWatcher *call);

private slots:
	void initProvider();
	void cleanProvider();
	void updateProperties();
	void updateProperty(const QString&, const QDBusVariant&);
	void emitSubscribeFinished();

	void deleteProperties();
	bool activateModem(const QString &);

	void updateSimPresent(const QDBusVariant &);
	void updateRegistrationStatus(const QDBusVariant &);
	void updateCellName(const QDBusVariant &);
	void updateSignalStrength(const QDBusVariant &);
	void updateTechnology(const QDBusVariant &);
	void updateNetworkName(const QDBusVariant &);

	void setUnknown(const QString&);

	void addModem(const QDBusObjectPath &, const QVariantMap&);
	void removeModem(const QDBusObjectPath &);




private:
	Manager *managerProxy;
	QString activeModem;
	QMap<QString,QVariant> properties;
	QSet<QString> subscribedProperties;
	NetworkProperties *networkProperties;
	SimProperties *simProperties;

};


#endif // CELLULARPROVIDER_H
