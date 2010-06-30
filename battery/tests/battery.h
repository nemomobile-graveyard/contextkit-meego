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

#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

class BatteryTest : public QObject
{
Q_OBJECT
public:
    explicit BatteryTest(QObject *parent = 0);

signals:

public slots:
    void valueChanged();
};

#endif // BATTERY_H
