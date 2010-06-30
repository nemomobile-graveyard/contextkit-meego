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

#ifndef COMMON_H
#define COMMON_H

#ifndef WANT_DEBUG
#define TRACE
#else
#include <QDebug>
#define TRACE qDebug()<<QString("[%1] %2(): %3").arg(__FILE__).arg(__func__).arg(__LINE__);
#endif

/*
 * Commonly used QRegExp expressions
 */
#include <QRegExp>
#define MATCH_ANY(p) QRegExp(p,Qt::CaseInsensitive,QRegExp::FixedString)
#define MATCH_ALL QRegExp()

QString stripLineID(QString lineid);

#endif // COMMON_H
