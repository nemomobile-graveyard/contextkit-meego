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

#include "mediaprovider.h"
#include <QDebug>
#include <QDBusConnection>
#include <QDBusReply>
#include <QStringList>
#include <QVariant>
#include <QString>

IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
        return new MediaProvider();
}

const QString MediaProvider::music("Media.NowPlaying");

MediaProvider::MediaProvider()
{
        qDebug() << "MediaProvider::MediaProvider()";

        QMetaObject::invokeMethod(this,"ready",Qt::QueuedConnection);

        musicPlayer = new Music("com.meego.app.music", "/com/meego/app/music", QDBusConnection::sessionBus(), 0);
        QObject::connect(musicPlayer, SIGNAL(currentTrackMetadataChanged(QStringList)),
            this, SLOT(getCurrentTrackMetadata()));

        getCurrentTrackMetadata();

        //sadly, QVariant is not a registered metatype
        qRegisterMetaType<QVariant>("QVariant");

        QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection,
                      Q_ARG(QString, music),
                      Q_ARG(QVariant, m_properties[music]));
}

MediaProvider::~MediaProvider()
{
    qDebug() << "MediaProvider::~MediaProvider()";
}

void MediaProvider::subscribe(QSet<QString> keys)
{
    qDebug() << "MediaProvider::subscribe(" << QStringList(keys.toList()).join(", ") << ")";

         m_subscribedProperties.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "emitChanged", Qt::QueuedConnection);
}

void MediaProvider::unsubscribe(QSet<QString> keys)
{
    qDebug() << "MediaProvider::unsubscribe(" << QStringList(keys.toList()).join(", ") << ")";
    m_subscribedProperties.subtract(keys);
}

void MediaProvider::emitSubscribeFinished()
{
        foreach(QString key, m_subscribedProperties){
                emit subscribeFinished(key);
        }
}

void MediaProvider::emitChanged()
{
  foreach (QString key, m_subscribedProperties) {
    emit valueChanged(key, QVariant(m_properties[key]));
  }
}

void MediaProvider::getCurrentTrackMetadata()
{
    QStringList musicprops = musicPlayer->getCurrentTrackMetadata();
    if(!musicPlayer->isValid() && musicprops.count() < 4){
        qDebug() << "No valid metadata for the music app";
        return;
    }
    qDebug() << "Music properties fetched " << musicprops;
    QVariantMap metadata;
    metadata["album"]= musicprops[0];
    metadata["artist"] = musicprops[1];
    metadata["title"]= musicprops[2];
    metadata["duration"]= musicprops[3].toInt();
    metadata["resource"] = ""; //tracker-uri object
    metadata["genre"] = ""; //string
    metadata["start-time"] = 0; //time
    /*if(musicPlayer->getPlaybackState() && musicPlayer->getPlaybackState().value() == 0)
        metadata["state"] = "unknown";
    else if(musicPlayer->getPlaybackState().value() == 1)
        metadata["state"] = "playing";
    else if(musicPlayer->getPlaybackState().value() == 2)
        metadata["state"] = "paused";
    else if(musicPlayer->getPlaybackState().value() == 3)
        metadata["state"] = "stopped";
    else if(musicPlayer->getPlaybackState().value() == 4)
        metadata["state"] = "fastforward";
    else if(musicPlayer->getPlaybackState().value() == 5)
        metadata["state"] = "rewind"; */
    m_properties[music] = metadata;
    if (m_subscribedProperties.contains(music))
        emitChanged();
}
