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

#ifndef CONNMANPROVIDER_H
#define CONNMANPROVIDER_H

#include <iproviderplugin.h>
#include <networkmanager.h>
#include <QSet>
#include <QMap>
#include <QString>
#include <QVariant>


using ContextSubscriber::IProviderPlugin;

extern "C"
{
  IProviderPlugin* pluginFactory(const QString& constructionString);
}

class ConnmanProvider : public IProviderPlugin
{
  Q_OBJECT;

public:
  ConnmanProvider();
  virtual ~ConnmanProvider();

  virtual void subscribe(QSet<QString> keys);
  virtual void unsubscribe(QSet<QString> keys);
  virtual void blockUntilReady() {}
  virtual void blockUntilSubscribed(const QString&) {}

  static const QString networkType;
  static const QString networkState;
  static const QString networkName;
  static const QString signalStrength;
  static const QString trafficIn;
  static const QString trafficOut;

protected:
  void timerEvent(QTimerEvent* event);

private:
  QString map(const QString &input) const;
  ;
  QSet<QString> m_subscribedProperties;
  QVariantMap m_properties;
  NetworkManager *m_networkManager;
  NetworkService *activeService;
  int m_timerId;
  QMap<QString, QString> m_nameMapper;

private slots:
  void emitSubscribeFinished();
  void emitChanged();
  void defaultRouteChanged(NetworkService* item);
  void stateChanged(QString State);
  void signalStrengthChanged(uint);
  void nameChanged(const QString &name);
};

#endif //CONNMANPROVIDER_H
