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

#include "cellularprovider.h"
#include "ofono_interface.h"
#include "manager_interface.h"

#include <QDBusConnection>

#define SignalStrength "Cellular.SignalStrength"
#define DataTechnology "Cellular.DataTechnology"
#define RegistrationStatus "Cellular.RegistrationStatus"
IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new CellularProvider();
}

CellularProvider::CellularProvider()
{
	qDebug() << "CellularProvider" << "Initializing cellular provider";
	QMetaObject::invokeMethod(this,"ready",Qt::QueuedConnection);
}

CellularProvider::~CellularProvider()
{

}

void CellularProvider::subscribe(QSet<QString> keys)
{
	if(subscribedProps.isEmpty()) initProvider();

	subscribedProps.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void CellularProvider::unsubscribe(QSet<QString> keys)
{
	subscribedProps.subtract(keys);
	if(subscribedProps.isEmpty()) cleanProvider();
}

void CellularProvider::initProvider()
{
	qDebug() << "CellularProvider" << "First subscriber appeared, connecting to ofono";
	
        Manager managerProxy("org.ofono",
				  "/",
				QDBusConnection::systemBus());
	QVariantMap managerProps = managerProxy.GetProperties();
        QList<QDBusObjectPath> paths = qdbus_cast<QList<QDBusObjectPath> >(managerProps["Modems"]);

        QString modemPath;
	foreach (QDBusObjectPath p, paths)
	{
		if (modemPath.isNull() || modemPath.isEmpty())
		{
			modemPath = QString(p.path());
			networkProps = new NetworkProperties("org.ofono",
					modemPath,
					QDBusConnection::systemBus());

			connect(networkProps,SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),this,SLOT(updateProperty(const QString&, const QDBusVariant&)));
			updateProperties();
			break;
		}
	}
	if (!networkProps)
	{
		qDebug() << "ProviderProvider" << "No NetworkRegistration interface found";
	}
}

void CellularProvider::cleanProvider()
{
	qDebug() << "CellularProvider" << "Last subscriber gone, destroying CellularProvider connections";
	delete networkProps;
	networkProps = NULL;
}

void CellularProvider::updateProperty(const QString &key, const QDBusVariant &val)
{
	qDebug() << "CellularProvider" << key<<" changed";
	if(key == "Status")
	{
		QString status = qdbus_cast<QString> (val.variant());
		if (status == "registered" || status == "roming")
		{
        		props[RegistrationStatus] = QVariant("online");
		} else if (status == "unknown")
		{
			props[RegistrationStatus] = QVariant("no-sim");
		} else if (status == "denied")
		{
			props[RegistrationStatus] = QVariant("forbidden");
		} else if (status == "unregister")
		{
			props[RegistrationStatus] = QVariant("offline");
		} else 
		{
			props[RegistrationStatus] =QVariant("");
		}
		
	} else if (key == "Strength")
	{
        	props[SignalStrength] = QVariant((val.variant()).toInt());
	} else if (key == "Technologies")
	{
        	props[DataTechnology] = val.variant();
	}
	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, props[key]);
	}
}

void CellularProvider::updateProperties()
{
	if(!networkProps) return;

	QVariantMap newProps = networkProps->GetProperties();
	props[SignalStrength] = QVariant(newProps["Strength"].toInt());
	props[DataTechnology] = newProps["Technologies"];

	QString status  = newProps["Status"].toString();
	if (status == "registered" || status == "roming")
	{
       		props[RegistrationStatus] = QVariant("online");
	} else if (status == "unknown")
	{
		props[RegistrationStatus] = QVariant("no-sim");
	} else if (status == "denied")
	{
		props[RegistrationStatus] = QVariant("forbidden");
	} else if (status == "unregister")
	{
		props[RegistrationStatus] = QVariant("offline");
	} else 
	{
		props[RegistrationStatus] =QVariant("");
	}

	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, props[key]);
	}
}
void CellularProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProps)
	{
		emit subscribeFinished(key, props[key]);
	}
}


