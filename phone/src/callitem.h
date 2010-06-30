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

#ifndef CALLITEM_H
#define CALLITEM_H

#include "callitemmodel.h"
#include <QString>
#include <QDateTime>
#include <QtDBus>
#include <QObject>

class CallItem : public QObject
{
	Q_OBJECT

public:
	CallItem(const QString path = QString());
	virtual ~CallItem();

	QString path() const;
	QString lineID() const;
	CallItemModel::CallState state() const;
	CallItemModel::CallDirection direction() const;
	CallItemModel::CallDisconnectReason reason() const;
	int duration() const;
	QDateTime startTime() const;
	CallProxy *callProxy() const;
	bool isValid();
	bool isValid() const;
	bool isActive();

public Q_SLOTS:
	void init();
	bool setPath(QString path);  // Setting this will create the CallProxy
	void setDirection(CallItemModel::CallDirection direction);
	void click();

Q_SIGNALS:
	// TODO: handle tap-and-hold
	void clicked();
	void stateChanged();

private Q_SLOTS:
	void callStateChanged(QString &);
	void callDisconnected(const QString &reason);

private:

	QString               m_path;
	QString m_lineid;
	CallItemModel::CallState  m_state;
	CallItemModel::CallDirection m_direction;
	int m_duration;
	QDateTime m_starttime;
	CallItemModel::CallDisconnectReason m_reason;
    
	Q_DISABLE_COPY(CallItem)
};

#endif // CALLITEM_H
