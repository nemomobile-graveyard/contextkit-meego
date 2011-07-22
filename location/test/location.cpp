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

#include "location.h"
#include <contextsubscriber/contextproperty.h>
#include <QtDebug>

LocationTest::LocationTest(QObject *parent) :
    QObject(parent)
{
	ContextProperty *property = new ContextProperty("Location.SatPositioningState",this);

	property->subscribe();
	property->waitForSubscription();

	connect(property,SIGNAL(valueChanged()),this,SLOT(valueChanged()));

	qDebug()<<"initial value: "<<property->value().toString();
}

void LocationTest::valueChanged()
{
	ContextProperty *prop = qobject_cast<ContextProperty*>(sender());

	if(!prop) return;

	qDebug()<<"new value: "<<prop->value().toString();
}
