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

#ifndef MEDIAPROVIDER_H
#define MEDIAPROVIDER_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <iproviderplugin.h>
#include <contextproperty.h>
#include "music_interface.h"

using ContextSubscriber::IProviderPlugin;

extern "C"
{
	IProviderPlugin* pluginFactory(const QString& constructionString);
}

class MediaProvider : public IProviderPlugin
{
	Q_OBJECT

public:
        MediaProvider();
        virtual ~MediaProvider();

	virtual void subscribe(QSet<QString> keys);
	virtual void unsubscribe(QSet<QString> keys);
	virtual void blockUntilReady() {}
	virtual void blockUntilSubscribed(const QString&) {}

        static const QString music;

private:
        QSet<QString> m_subscribedProperties;
        QVariantMap m_properties;
        Music *musicPlayer; ///< The interface to the music app

private slots:
        void emitSubscribeFinished();
        void emitChanged();
        void getCurrentTrackMetadata();
};

#endif // MEDIAPROVIDER_H
