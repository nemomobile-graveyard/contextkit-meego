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

//#include "common.h"
#include "callitem.h"
#include "callitemmodel.h"
#include <QDebug>


CallItem::CallItem(const QString path )
      :m_path(path)
{
	if (isValid())
		init();
}

CallItem::~CallItem()
{
}

void CallItem::init()
{
	if (!m_path.isEmpty()) {
		CallProxy *call = new CallProxy(m_path);

		if (call->isValid()) {
			connect(call,SIGNAL(stateChanged(QString &)),this,SLOT(callStateChanged(QString &)));
		} else
			qCritical("Invalid CallProxy instance!");
	} else
		qCritical("Empty call path.  Can not create CallProxy!");
}

bool CallItem::isValid()
{
	return (!path().isEmpty());
}

bool CallItem::isValid() const
{
	return (!path().isEmpty());
}

QString CallItem::path() const
{
	return m_path;
}

bool CallItem::setPath(QString path)
{
	if (!m_path.isEmpty()) {
		qCritical("Path already set and can not be changed once it is set");
		return false;
	} else if (path.isEmpty()) {
		qCritical("It makes no sense to set Path to an empty string!?!?");
		return false;
	}

	m_path = path;

	init();

	return true;
}

void CallItem::setDirection(CallItemModel::CallDirection direction)
{
	m_direction = direction; 
}

QString CallItem::lineID() const
{
	return (isValid())?m_lineid:QString();
}

CallItemModel::CallState CallItem::state() const
{
	return m_state;
}

CallItemModel::CallDirection CallItem::direction() const
{
	return m_direction;
}

CallItemModel::CallDisconnectReason CallItem::reason() const
{
	return m_reason;
}

int CallItem::duration() const
{
	return m_duration;
}

QDateTime CallItem::startTime() const
{
	return m_starttime;
}

void CallItem::click()
{
	emit clicked();
}

void CallItem::callStateChanged(QString &state)
{
	qDebug()<<"CallItem: callStateChanged :"<<state;

	if (state == "active")
		m_state = CallItemModel::STATE_ACTIVE;
	else if (state == "held")
		m_state = CallItemModel::STATE_HELD;
	else if (state == "dialing")
		m_state = CallItemModel::STATE_DIALING;
	else if (state == "alerting")
		m_state = CallItemModel::STATE_ALERTING;
	else if (state == "incoming")
		m_state = CallItemModel::STATE_INCOMING;
	else if (state == "waiting")
		m_state = CallItemModel::STATE_WAITING;
	else if (state == "disconnected")
		m_state = CallItemModel::STATE_DISCONNECTED;
	else
		m_state = CallItemModel::STATE_NONE;  

	qDebug()<<"CallItem: callstatechanged m_statec hanged:"<<m_state;
	emit stateChanged();
}

void CallItem::callDisconnected(const QString &reason)
{
	Q_UNUSED(reason);
}

bool CallItem::isActive()
{
	return (m_state == CallItemModel::STATE_ACTIVE);
}
