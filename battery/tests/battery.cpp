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

#include "battery.h"
#include <contextsubscriber/contextproperty.h>
#include <QtDebug>

BatteryTest::BatteryTest(QObject *parent) :
    QObject(parent)
{
	ContextProperty *property = new ContextProperty("Battery.OnBattery",this);

	property->subscribe();
	property->waitForSubscription();

	connect(property,SIGNAL(valueChanged()),this,SLOT(valueChanged()));

	qDebug()<<"initial value: "<<property->value().toBool();
}

void BatteryTest::valueChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new value: "<<prop->value().toBool();
}
