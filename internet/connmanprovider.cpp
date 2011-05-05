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

  findActiveWifiConnection();

  connect(m_networkListModel, SIGNAL(defaultTechnologyChanged(QString)),
	  this, SLOT(defaultTechnologyChanged(QString)));
  connect(m_networkListModel, SIGNAL(stateChanged(QString)),
	  this, SLOT(stateChanged(QString)));
  connect(m_networkListModel, SIGNAL(connectedNetworkItemsChanged()),
          this, SLOT(findActiveWifiConnection()));

  connect(m_networkListModel, SIGNAL(countChanged(int)),
          this, SLOT(countChangedSlot(int)));
  
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

void ConnmanProvider::findActiveWifiConnection()
{
  qDebug()<<"trying to find active wifi connection...";
  qDebug()<<"searching "<<m_networkListModel->networks().count()<<"items";
  foreach(NetworkItemModel* item, m_networkListModel->networks()) {
      qDebug()<<"connection: "<<item->type()<<" "<<(int)item->state()<<" >= "<<(int)NetworkItemModel::StateReady;
      if(item->type() == "wifi" && (int)item->state() >= (int)NetworkItemModel::StateReady) {
         qDebug()<<"found a connected wifi service!! "<<item->name()<<" strength: "<<item->strength();

         if(item == activeWifi) return;

         activeWifi->disconnect(this);
         activeWifi = item;

         m_properties[signalStrength] = item->strength();
         valueChanged(signalStrength,m_properties[signalStrength]);
         connect(activeWifi,SIGNAL(strengthChanged(int)),this,SLOT(signalStrengthChanged(int)));
         return;
      }
  }
  activeWifi = NULL;
  m_properties[signalStrength] = 0;
}

void ConnmanProvider::signalStrengthChanged(int strength)
{
    if(!activeWifi) return;
    qDebug()<<"signal strength for: "<<activeWifi->name()<<" "<<strength;
    m_properties[signalStrength] = strength;
    valueChanged(signalStrength,m_properties[signalStrength]);
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
    if(item)
        m_properties[signalStrength] = item->strength();
    else
        m_properties[signalStrength] = 0;

    if (m_subscribedProperties.contains(networkType)) {
      emit valueChanged(signalStrength, QVariant(m_properties[signalStrength]));
    }
}

void ConnmanProvider::stateChanged(QString State)
{
  m_properties[networkState] = map(State);
  if (m_subscribedProperties.contains(networkState)) {
    emit valueChanged(networkState, QVariant(m_properties[networkState]));
  }
}
