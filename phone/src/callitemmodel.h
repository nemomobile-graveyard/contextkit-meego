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

#ifndef CALLITEMMODEL_H
#define CALLITEMMODEL_H

#include <QString>
#include <QDateTime>
#include "callproxy.h"

class CallItemModel
{

public:
    enum CallState {
        STATE_NONE = 0,
        STATE_ACTIVE,
        STATE_HELD,
        STATE_DIALING,
        STATE_ALERTING,
        STATE_INCOMING,
        STATE_WAITING,
        STATE_DISCONNECTED,
        STATE_LAST,
    };

    enum CallDirection {
        DIRECTION_NONE = 0,
        DIRECTION_IN,
        DIRECTION_OUT,
        DIRECTION_MISSED,
        DIRECTION_LAST,
    };

    enum CallDisconnectReason {
        DISCONNECT_NONE = 0,
        DISCONNECT_LOCAL,
        DISCONNECT_REMOTE,
        DISCONNECT_NETWORK,
        DISCONNECT_LAST,
    };

};

#endif // CALLITEMMODEL_H
