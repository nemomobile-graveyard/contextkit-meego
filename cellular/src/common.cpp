#include "common.h"

QDBusArgument & operator << (QDBusArgument &argument,
                             const OfonoPathProperties &d)
{
    argument.beginStructure();
    argument << d.path << d.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument & operator >> (const QDBusArgument &argument,
                                   OfonoPathProperties &d)
{
    argument.beginStructure();
    argument >> d.path >> d.properties;
    argument.endStructure();
    return argument;
}
