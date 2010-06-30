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

#include "cellulartest.h"
#include <contextsubscriber/contextproperty.h>
#include <QtDebug>
#include <QStringList>

CellularTest::CellularTest(QObject *parent) :
    QObject(parent)
{
	ContextProperty *propSignal = new ContextProperty("Cellular.SignalStrength",this);
	ContextProperty *propStatus = new ContextProperty("Cellular.RegistrationStatus",this);
	ContextProperty *propTech = new ContextProperty("Cellular.DataTechnology",this);

	connect(propSignal,SIGNAL(valueChanged()),this,SLOT(signalChanged()));
	connect(propStatus,SIGNAL(valueChanged()),this,SLOT(statusChanged()));
	connect(propTech,SIGNAL(valueChanged()),this,SLOT(technologyChanged()));

        QString list = (propTech->value()).toString();
	qDebug()<<"initial techonology values: "<<list;
	qDebug()<<"initial signal status value: "<<propSignal->value().toInt();
	qDebug()<<"initial registration status value: "<<propStatus->value().toString();
}

void CellularTest::statusChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new registrationstatus: "<<prop->value().toString();
}

void CellularTest::signalChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new signal value: "<<prop->value().toInt();
	qDebug()<<"system-ui new signal value: "<<prop->value().toDouble() * 0.01f;
}

void CellularTest::technologyChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new  technology value: "<<prop->value().toString();
}
