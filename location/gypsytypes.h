#ifndef GYPSYTYPES_H
#define GYPSYTYPES_H

#include <QObject>

class SatInfo
{
public:

	int id;
	bool inUse;
	int elevation;
	int azimuth;
	int snr;
};

typedef QList<SatInfo> SatInfoArray;

Q_DECLARE_METATYPE(SatInfoArray)

#endif // GYPSYTYPES_H
