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

private slots:
	void initProvider();
	void cleanProvider();
	void updateProperties();
	void updateProperty(const QString&, const QDBusVariant&);
	void emitSubscribeFinished();

private:
	QMap<QString,QVariant> props;
	QSet<QString> subscribedProps;
	NetworkProperties *networkProps; 
};


#endif // CELLULARPROVIDER_H
