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

#include "bluetoothprovider.h"
#include <QDebug>
#include <QStringList>
#include <QVariant>

IProviderPlugin* pluginFactory(const QString& constructionString)
{
    Q_UNUSED(constructionString)
    return new BluetoothProvider();
}

const QString BluetoothProvider::connected("Bluetooth.Connected");


BluetoothProvider::BluetoothProvider()
{
  qDebug() << "BluetoothProvider::BluetoothProvider()";

  QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);

  m_bluetoothDevices = new BluetoothDevicesModel(this);
  m_properties[connected] = m_bluetoothDevices->connected();

  connect(m_bluetoothDevices, SIGNAL(connectedChanged(bool)),
      this, SLOT(connectedChanged(bool)));

  
  //sadly, QVariant is not a registered metatype
  qRegisterMetaType<QVariant>("QVariant");

  QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
                Q_ARG(QString, connected),
                Q_ARG(QVariant, m_properties[connected]));

}

BluetoothProvider::~BluetoothProvider()
{
  qDebug() << "BluetoothProvider::~BluetoothProvider()";
}

void BluetoothProvider::subscribe(QSet<QString> keys)
{
  qDebug() << "BluetoothProvider::subscribe(" << QStringList(keys.toList()).join(", ") << ")";

  m_subscribedProperties.unite(keys);

  QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
  QMetaObject::invokeMethod(this, "emitChanged", Qt::QueuedConnection);
}

void BluetoothProvider::unsubscribe(QSet<QString> keys)
{
  qDebug() << "BluetoothProvider::unsubscribe(" << QStringList(keys.toList()).join(", ") << ")";
  
  m_subscribedProperties.subtract(keys);
}


void BluetoothProvider::emitSubscribeFinished()
{
  foreach (QString key, m_subscribedProperties) {
    emit subscribeFinished(key);
  }
}

void BluetoothProvider::emitChanged()
{
  foreach (QString key, m_subscribedProperties) {
    emit valueChanged(key, QVariant(m_properties[key]));
  }
}

void BluetoothProvider::connectedChanged(bool conntected)
{
    m_properties[connected] = m_bluetoothDevices->connected();
    if (m_subscribedProperties.contains(connected))
        emitChanged();
}
