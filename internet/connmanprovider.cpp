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

#include "connmanprovider.h"
#include <QDebug>
#include <QStringList>
#include <QVariant>

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


ConnmanProvider::ConnmanProvider(): activeWifi(NULL)
{
  qDebug() << "ConnmanProvider::ConnmanProvider()";

  m_nameMapper["wifi"] = "WLAN";
  m_nameMapper["gprs"] = "GPRS";
  m_nameMapper["edge"] = "GPRS";
  m_nameMapper["umts"] = "GPRS";
  m_nameMapper["ethernet"] = "ethernet";

  m_nameMapper["offline"] = "disconnected";
  m_nameMapper["online"] = "connected";
  m_nameMapper["connected"] = "connected";

  //hack
  m_properties[trafficIn] = 20;
  m_properties[trafficOut] = 20;
  m_timerId = startTimer(5*1000);
  
  QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);

  m_networkListModel = new NetworkListModel();
  m_properties[networkType] = map(m_networkListModel->defaultTechnology());
  m_properties[networkState] = map(m_networkListModel->state());

  if(m_networkListModel->defaultRoute())
  {
      m_properties[signalStrength] = m_networkListModel->defaultRoute()->strength();
      m_properties[networkName] = m_networkListModel->defaultRoute()->name();
  }

  connect(m_networkListModel, SIGNAL(defaultTechnologyChanged(QString)),
	  this, SLOT(defaultTechnologyChanged(QString)));
  connect(m_networkListModel, SIGNAL(stateChanged(QString)),
	  this, SLOT(stateChanged(QString)));
  connect(m_networkListModel, SIGNAL(defaultRouteChanged(NetworkItemModel*)),
          this, SLOT(defaultRouteChanged(NetworkItemModel*)));
  
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
  qDebug() << "ConnmanProvider::~ConnmanProvider()";
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

void ConnmanProvider::signalStrengthChanged(int strength)
{
    if(!activeWifi) return;

    qDebug()<<"signal strength for: "<<activeWifi->name()<<" "<<strength;

    m_properties[signalStrength] = strength;

    if (m_subscribedProperties.contains(signalStrength)) {
      emit valueChanged(signalStrength, QVariant(m_properties[signalStrength]));
    }
}

void ConnmanProvider::emitSubscribeFinished()
{
  foreach (QString key, m_subscribedProperties) {
    emit subscribeFinished(key);
  }
}

void ConnmanProvider::emitChanged()
{
  foreach (QString key, m_subscribedProperties) {
    emit valueChanged(key, QVariant(m_properties[key]));
  }
}

void ConnmanProvider::defaultTechnologyChanged(QString Technology)
{
  //qDebug() << "defaultTechnologyChanged: " << Technology;
  m_properties[networkType] = map(Technology);
  if (m_subscribedProperties.contains(networkType)) {
    emit valueChanged(networkType, QVariant(m_properties[networkType]));
  }
}

void ConnmanProvider::defaultRouteChanged(NetworkItemModel *item)
{
    if(activeWifi)
    {
        activeWifi->disconnect(this,SLOT(signalStrengthChanged(int)));
    }

    activeWifi = item;

    if(item)
    {
        m_properties[networkName] = item->name();

        m_properties[signalStrength] = item->strength();
        connect(activeWifi,SIGNAL(strengthChanged(int)),this,SLOT(signalStrengthChanged(int)));
    }
    else
        m_properties[signalStrength] = 0;

    if (m_subscribedProperties.contains(signalStrength)) {
      emit valueChanged(signalStrength, QVariant(m_properties[signalStrength]));
    }

    if (m_subscribedProperties.contains(networkName)) {
      emit valueChanged(networkName, QVariant(m_properties[networkName]));
    }
}

void ConnmanProvider::stateChanged(QString State)
{
  m_properties[networkState] = map(State);
  if (m_subscribedProperties.contains(networkState)) {
    emit valueChanged(networkState, QVariant(m_properties[networkState]));
  }
}
