/*  -*- Mode: C++ -*-
 *
 * contextkit-meego
 * Copyright © 2010, Intel Corporation.
 * Copyright © 2012, Jolla.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "connmanprovider.h"
#include <QDebug>
#include <QStringList>
#include <QVariant>

#define DBG qDebug() << __FILE__ << ":" << __LINE__ << ":"

IProviderPlugin* pluginFactory(const QString& constructionString)
{
  Q_UNUSED(constructionString)
    return new ConnmanProvider();
}

const QString ConnmanProvider::networkType("Internet.NetworkType");
const QString ConnmanProvider::networkState("Internet.NetworkState");
const QString ConnmanProvider::networkName("Internet.NetworkName");
const QString ConnmanProvider::signalStrength("Internet.SignalStrength");
const QString ConnmanProvider::trafficIn("Internet.TrafficIn");
const QString ConnmanProvider::trafficOut("Internet.TrafficOut");


ConnmanProvider::ConnmanProvider(): activeService(NULL)
{
  DBG << "ConnmanProvider::ConnmanProvider()";

  m_nameMapper["wifi"] = "WLAN";
  m_nameMapper["gprs"] = "GPRS";
  m_nameMapper["edge"] = "GPRS";
  m_nameMapper["umts"] = "GPRS";
  m_nameMapper["ethernet"] = "ethernet";

  m_nameMapper["offline"] = "disconnected";
  m_nameMapper["idle"] = "disconnected";
  m_nameMapper["online"] = "connected";
  m_nameMapper["ready"] = "connected";

  //hack
  m_properties[trafficIn] = 20;
  m_properties[trafficOut] = 20;
  m_timerId = startTimer(5*1000);

  QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);

  m_networkManager = NetworkManagerFactory::createInstance();;
  m_properties[networkType] = map("gprs");
  m_properties[networkState] = map(m_networkManager->state());

  if(m_networkManager->defaultRoute())
  {
      activeService = m_networkManager->defaultRoute();

      connect(activeService,SIGNAL(nameChanged(QString)),this,SLOT(nameChanged(QString)));
      connect(activeService,SIGNAL(strengthChanged(uint)),this,SLOT(signalStrengthChanged(uint)));

      m_properties[signalStrength] = m_networkManager->defaultRoute()->strength();
      m_properties[networkName] = m_networkManager->defaultRoute()->name();
      m_properties[networkType] = map(activeService->type());
  }

  connect(m_networkManager, SIGNAL(stateChanged(QString)),
	  this, SLOT(stateChanged(QString)));
  connect(m_networkManager, SIGNAL(defaultRouteChanged(NetworkService*)),
          this, SLOT(defaultRouteChanged(NetworkService*)));

  //sadly, QVariant is not a registered metatype
  qRegisterMetaType<QVariant>("QVariant");

  QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
			    Q_ARG(QString, networkType),
			    Q_ARG(QVariant, m_properties[networkType]));
  QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
			    Q_ARG(QString, networkState),
			    Q_ARG(QVariant, m_properties[networkState]));
  QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
                Q_ARG(QString, signalStrength),
                Q_ARG(QVariant, m_properties[signalStrength]));
  QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
                Q_ARG(QString, networkName),
                Q_ARG(QVariant, m_properties[networkName]));
}

ConnmanProvider::~ConnmanProvider()
{
  DBG << "ConnmanProvider::~ConnmanProvider()";
}

void ConnmanProvider::subscribe(QSet<QString> keys)
{
  qDebug() << "ConnmanProvider::subscribe(" << QStringList(keys.toList()).join(", ") << ")";

  m_subscribedProperties.unite(keys);

  QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
  QMetaObject::invokeMethod(this, "emitChanged", Qt::QueuedConnection);
}

void ConnmanProvider::unsubscribe(QSet<QString> keys)
{
  qDebug() << "ConnmanProvider::unsubscribe(" << QStringList(keys.toList()).join(", ") << ")";

  m_subscribedProperties.subtract(keys);
}

void ConnmanProvider::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);
  m_properties[trafficIn] = qrand()*10.0 / RAND_MAX + 20;
  m_properties[trafficOut] = qrand()*10.0 / RAND_MAX + 20;

  emit valueChanged(trafficIn, m_properties[trafficIn]);
  emit valueChanged(trafficOut, m_properties[trafficOut]);
}

QString ConnmanProvider::map(const QString &input) const
{
  return m_nameMapper[input];
}

void ConnmanProvider::signalStrengthChanged(uint strength)
{
    if(!activeService) return;

    m_properties[signalStrength] = strength;

    if (m_subscribedProperties.contains(signalStrength)) {
      emit valueChanged(signalStrength, QVariant(m_properties[signalStrength]));
    }
}

void ConnmanProvider::emitSubscribeFinished()
{
  foreach (QString key, m_subscribedProperties) {
    DBG << "emit subscribedFinished(" << key << ")";
    emit subscribeFinished(key);
  }
}

void ConnmanProvider::emitChanged()
{
  foreach (QString key, m_subscribedProperties) {
    DBG << "ConnmanProvider::emitChanged(): " << key << "=" << m_properties[key];
    emit valueChanged(key, QVariant(m_properties[key]));
  }
}

void ConnmanProvider::defaultRouteChanged(NetworkService *item)
{
    if(activeService)
    {
        DBG << "disconnecting from " << activeService->name();
        activeService->disconnect(this,SLOT(nameChanged(QString)));
        activeService->disconnect(this,SLOT(signalStrengthChanged(uint)));
    }

    activeService = item;

    if (item) {
        DBG << "new default route: " << item->name();
        QString ntype = map(item->type());
        if (m_properties[networkType] != ntype) {
            if (m_subscribedProperties.contains(networkType)) {
                m_properties[networkType] = ntype;
                DBG << "networkType has changed to " << ntype;
                emit valueChanged(networkType, QVariant(m_properties[networkType]));
            }
        }

        m_properties[networkName] = item->name();
        m_properties[signalStrength] = item->strength();

        DBG << "connecting to " << activeService->name();
        connect(activeService,SIGNAL(strengthChanged(uint)),this,SLOT(signalStrengthChanged(uint)));
        connect(activeService,SIGNAL(nameChanged(QString)),this,SLOT(nameChanged(QString)));
    }
    else
        m_properties[signalStrength] = 0;

    if (m_subscribedProperties.contains(signalStrength)) {
      DBG << "emit valueChanged(strength)";
      emit valueChanged(signalStrength, QVariant(m_properties[signalStrength]));
    }

    if (m_subscribedProperties.contains(networkName)) {
      DBG << "emit valueChanged(naetworkName)";
      emit valueChanged(networkName, QVariant(m_properties[networkName]));
    }
}

void ConnmanProvider::nameChanged(const QString &name)
{
    DBG << "ConnmanProvider::nameChanged(" << name << ")";
    m_properties[networkName] = name;
    if (m_subscribedProperties.contains(networkName)) {
      emit valueChanged(networkName, QVariant(m_properties[networkName]));
    }
}

void ConnmanProvider::stateChanged(QString State)
{
  DBG << "ConnmanProvider::stateChanged(" << State << ")";
  m_properties[networkState] = map(State);
  if (m_subscribedProperties.contains(networkState)) {
    emit valueChanged(networkState, QVariant(m_properties[networkState]));
  }
}
