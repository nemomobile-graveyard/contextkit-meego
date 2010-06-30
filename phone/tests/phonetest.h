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

#ifndef CELLULARTEST_H
#define CELLULARTEST_H

#include <QObject>

class PhoneTest : public QObject
{
	Q_OBJECT
public:
	explicit PhoneTest(QObject *parent = 0);

signals:

public slots:
	void mutedChanged();
	void callChanged();
};

#endif // CELLULAR_H
