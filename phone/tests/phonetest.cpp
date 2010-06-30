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

#include "phonetest.h"
#include <contextsubscriber/contextproperty.h>
#include <QtDebug>

PhoneTest::PhoneTest(QObject *parent) :
    QObject(parent)
{
	ContextProperty *propMuted = new ContextProperty("Phone.Muted",this);
	ContextProperty *propCall = new ContextProperty("Phone.Call",this);

	connect(propMuted,SIGNAL(valueChanged()),this,SLOT(mutedChanged()));
	connect(propCall,SIGNAL(valueChanged()),this,SLOT(callChanged()));

	qDebug()<<"initial muted value: "<<propMuted->value().toBool();
	qDebug()<<"initial call value: "<<propCall->value().toString();
}

void PhoneTest::mutedChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new muted value: "<<prop->value().toBool();
}

void PhoneTest::callChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new call value: "<<prop->value().toString();
}
