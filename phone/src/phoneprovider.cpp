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

#include "phoneprovider.h"
#include "manager_interface.h"
#include "callvolume_interface.h"
#include "callmanager_interface.h"
#include <QDBusConnection>

#define Call "Phone.Call"
#define Muted "Phone.Muted"

IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new PhoneProvider();
}

PhoneProvider::PhoneProvider():m_currCalls(0),m_currMpartyCalls(0),m_currActiveCall(new CallItem(QString("")))
{
	qDebug() << "PhoneProvider" << "Initializing phone provider";
	registerContextDataTypes();
	QMetaObject::invokeMethod(this,"ready",Qt::QueuedConnection);
}

PhoneProvider::~PhoneProvider()
{

}

void PhoneProvider::subscribe(QSet<QString> keys)
{
	if(subscribedProps.isEmpty()) initProvider();

	subscribedProps.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void PhoneProvider::unsubscribe(QSet<QString> keys)
{
	subscribedProps.subtract(keys);
	if(subscribedProps.isEmpty()) cleanProvider();
}

void PhoneProvider::initProvider()
{
	qDebug() << "PhoneProvider" << "First subscriber appeared, connecting to CallVolume";
	
        Manager managerProxy("org.ofono",
				  "/",
				QDBusConnection::systemBus());

        QDBusPendingReply<QArrayOfPathProperties> reply;
        QDBusPendingCallWatcher * watcher;

        reply = managerProxy.GetModems();
        watcher = new QDBusPendingCallWatcher(reply);
        watcher->waitForFinished();

        QList<QDBusObjectPath> paths = providerProcessGetModems(watcher);

        QString modemPath;

	foreach (QDBusObjectPath p, paths)
	{
		if (modemPath.isNull() || modemPath.isEmpty())
		{
			modemPath = QString(p.path());

			volumeProps  = new CallVolume("org.ofono",
					modemPath,
					QDBusConnection::systemBus());

			callProps  = new CallManager(modemPath);

			connect(volumeProps,SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),this,SLOT(updateProperty(const QString&, const QDBusVariant&)));
			connect(callProps,SIGNAL(callsChanged()),this,SLOT(updateCall()));

			updateProperties();

			break;
		}
	}

	if (!volumeProps)
	{
		qDebug() << "PhoneProvider" << "Couldn't connect to CallVolume interface";
	}

	if (!callProps)
	{
		qDebug() << "PhoneProvider" << "Couldn't connect to CallMananger interface";
	}
}

QList<QDBusObjectPath> PhoneProvider::providerProcessGetModems(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QArrayOfPathProperties> reply = *call;
    QList<QDBusObjectPath> pathlist;
    if (reply.isError()) {
        // TODO: Handle this properly, by setting states, or disabling features
        qWarning() << "org.ofono.Manager.GetModems() failed: " <<
                      reply.error().message();
    } else {
        QArrayOfPathProperties modems = reply.value();
        qDebug() << QString("modem count:")<<modems.count();
        for (int i=0; i< modems.count();i++) {
            OfonoPathProperties p = modems[i];
            pathlist.append(QDBusObjectPath(p.path.path()));
        }
    }
    return pathlist;
}

void PhoneProvider::cleanProvider()
{
	qDebug() << "PhoneProvider" << "Last subscriber gone, destroying CellularProvider connections";

	delete callProps;
	callProps = NULL;
	
	delete volumeProps;
	volumeProps = NULL;
}

void PhoneProvider::updateProperty(const QString &key, const QDBusVariant &val)
{
	qDebug()<<"PhoneProvider:"<<key;

	if(key == "Muted")
	{
        	props[Muted] = val.variant();
	}
	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, props[key]);
	}
}

void PhoneProvider::updateCall()
{
	qDebug()<<"PhoneProvider: callChanged Signal received";

	if(!callProps) return;

	CallItem *prevActiveCall = m_currActiveCall;

	m_currActiveCall = callProps->activeCall();

	if(m_currActiveCall)
	{
		if (prevActiveCall)
		{
			// A new call is in active state
			if(prevActiveCall->path() != prevActiveCall->path())
			{
				qDebug()<<"Assigning Active";	
				props[Call] = QVariant("active");
			}

		}else {
			// call is in active state
			qDebug()<<"Assigning Active";	
			props[Call] = QVariant("active");
		}
	} else {
        	uint prevRingingCalls = m_currCalls+m_currMpartyCalls;
		qDebug()<<"updateCalls: Counting calls";
		m_currCalls = (callProps->calls()).count();
		m_currMpartyCalls = (callProps->multipartyCalls()).count();
		qDebug()<<"updateCall: prevRingingCalls:"<<prevRingingCalls;
		qDebug()<<"updateCall: currCalls:"<<m_currCalls;
		if (prevRingingCalls < (m_currCalls+m_currMpartyCalls))
			props[Call] = QVariant("ringing");
		else if (prevRingingCalls > (m_currCalls+m_currMpartyCalls))
                        props[Call] = QVariant("disconnected");
	}
	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, props[key]);
	}
}
void PhoneProvider::updateProperties()
{
	qDebug()<<"PhoneProvider: updateProperties";

	if(!volumeProps ) return;

	QVariantMap newProps = volumeProps->GetProperties();
	props[Muted] = newProps["Muted"];

	if(!callProps ) return;

	CallItem *prevActiveCall = m_currActiveCall;
	m_currActiveCall = callProps->activeCall();
	if (prevActiveCall && m_currActiveCall){
		if (prevActiveCall->path() != m_currActiveCall->path() )
			props[Call] = QVariant("active");
        }
	else {
		qDebug()<<"updateProperties activecall is null";	
        	uint prevRingingCalls = m_currCalls+m_currMpartyCalls;
		m_currCalls = (callProps->calls()).count();
		m_currMpartyCalls = (callProps->multipartyCalls()).count();
		qDebug()<<"ups: prevRingingCalls:"<<prevRingingCalls;
		qDebug()<<"ups: currRingingCalls:"<<m_currCalls;
		if (prevRingingCalls < (m_currCalls+m_currMpartyCalls))
			props[Call] = QVariant("ringing");
		else if (prevRingingCalls > (m_currCalls+m_currMpartyCalls))
                        props[Call] = QVariant("disconnected");
	}

	foreach(QString key, subscribedProps)
	{
		emit valueChanged(key, props[key]);
	}
}
void PhoneProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProps)
	{
		emit subscribeFinished(key, props[key]);
	}
}
