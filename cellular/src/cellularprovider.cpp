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

#include "cellularprovider.h"
#include "ofono_interface.h"
#include "sim_interface.h"

#include <QDBusConnection>

#define SignalStrength "Cellular.SignalStrength"
#define Technology "Cellular.Technology"
#define DataTechnology "Cellular.DataTechnology"
#define RegistrationStatus "Cellular.RegistrationStatus"
#define CellName "Cellular.CellName"
#define NetworkName "Cellular.NetworkName"
#define SignalBars "Cellular.SignalBars" // DUMMY
#define PacketData "Cellular.PacketData" // TODO
#define GSMBand "Cellular.GSMBand" // TODO
#define ExtendedNName "Cellular.ExtendedNetworkName" // TODO
#define CellNameEnabled "Cellular.CellNameDisplayEnabled" // TODO


IProviderPlugin* pluginFactory(const QString& constructionString)
{
	Q_UNUSED(constructionString)
	return new CellularProvider();
}

CellularProvider::CellularProvider(): activeModem(""), networkProperties(NULL), simProperties(NULL)
{
	qDebug() << "CellularProvider" << "Initializing cellular provider";
	registerContextDataTypes();
	QMetaObject::invokeMethod(this,"ready",Qt::QueuedConnection);
}

CellularProvider::~CellularProvider()
{
	cleanProvider();
}

void CellularProvider::subscribe(QSet<QString> keys)
{
	if(subscribedProperties.isEmpty()) initProvider();

	subscribedProperties.unite(keys);

	QMetaObject::invokeMethod(this, "emitSubscribeFinished", Qt::QueuedConnection);
}

void CellularProvider::unsubscribe(QSet<QString> keys)
{
	subscribedProperties.subtract(keys);
	if(subscribedProperties.isEmpty()) cleanProvider();
}

void CellularProvider::initProvider()
{
	QDBusPendingReply<QArrayOfPathProperties> reply;
	QList<QDBusObjectPath> paths;

	qDebug() << "CellularProvider" << "First subscriber appeared, connecting to ofono";
	
	 managerProxy = new Manager("org.ofono", "/", QDBusConnection::systemBus());

	reply = managerProxy->GetModems();
	QDBusPendingCallWatcher watcher(reply);
	watcher.waitForFinished();

	connect(managerProxy, SIGNAL(ModemAdded(const QDBusObjectPath &, const QVariantMap &)),
		this, SLOT(addModem(const QDBusObjectPath &, const QVariantMap &)));

	connect(managerProxy, SIGNAL(ModemRemoved(const QDBusObjectPath&)),
		this, SLOT(removeModem(const QDBusObjectPath&)));

	paths = providerProcessGetModems(&watcher);

	foreach (QDBusObjectPath modem, paths)
	{
		if (activateModem(modem.path())) {
			updateProperties();
			return;
		}
	}

	qDebug() << "ProviderProvider" << "No NetworkRegistration interface found";

}

QList<QDBusObjectPath> CellularProvider::providerProcessGetModems(QDBusPendingCallWatcher *call)
{
	QDBusPendingReply<QArrayOfPathProperties> reply = *call;
	QList<QDBusObjectPath> pathlist;

	if (reply.isError()) {
		// TODO: Handle this properly, by setting states, or disabling features
		qWarning() << "org.ofono.Manager.GetModems() failed: " <<
		reply.error().message();
	} else {
		QArrayOfPathProperties modems = reply.value();
		qDebug() << QString("modem count:")<<modems.count();

		for (int i=0; i< modems.count();i++) {
			OfonoPathProperties p = modems[i];
			pathlist.append(QDBusObjectPath(p.path.path()));
		}
	}
	return pathlist;
}

bool CellularProvider::activateModem(const QString& path) {

	qDebug() << "CellularProvider" << "activateModem" << path;

	networkProperties = new NetworkProperties("org.ofono",
				path,
				QDBusConnection::systemBus());
	simProperties = new SimProperties("org.ofono",
				path,
				QDBusConnection::systemBus());

	if (!networkProperties->isValid() || !simProperties->isValid()) {
		deleteProperties();
		return false;
	}

	connect(networkProperties,SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),
		this,SLOT(updateProperty(const QString&, const QDBusVariant&)));

	connect(simProperties,SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),
		this,SLOT(updateProperty(const QString&, const QDBusVariant&)));

	activeModem = path;

	return true;
}

void CellularProvider::addModem(const QDBusObjectPath& path, const QVariantMap& properties) {

	Q_UNUSED(properties)

	if(activeModem != "")
		return;

	QString modemPath = path.path();

	if (activateModem(modemPath) == false)
		return;

	updateProperties();

	qDebug() << "CellularProvider" << "Modem added: " << path.path();
}

void CellularProvider::removeModem(const QDBusObjectPath& path) {

	qDebug() << "CellularProvider" << "removeModem" << path.path();

	if(path.path() == activeModem) {
		activeModem = "";

		deleteProperties();

		QArrayOfPathProperties modems = managerProxy->GetModems();
		foreach (OfonoPathProperties modem, modems) {

			if (activateModem(modem.path.path())) {
				updateProperties();
				return;
			}
		}

		qDebug() << "CellularProvider" << "No proper modem found";
	}
}


void CellularProvider::deleteProperties() {

	qDebug() << "CellularProvider" << "deleteProperties";

	delete networkProperties;
	delete simProperties;

	foreach (QString prop, subscribedProperties) {
		setUnknown(prop);
	}
	properties.empty();
	networkProperties = NULL;
	simProperties = NULL;
}

void CellularProvider::cleanProvider()
{
	qDebug() << "CellularProvider" << "Last subscriber gone, destroying CellularProvider connections";
	deleteProperties();

	delete managerProxy;
	managerProxy = NULL;
}

void CellularProvider::updateProperty(const QString &key, const QDBusVariant &val)
{
	qDebug() << "CellularProvider" << key<<" changed";

	if (key == "Present")
		updateSimPresent(val);

	else if (key == "Status")
		updateRegistrationStatus(val);

	else if (key == "BaseStation")
		updateCellName(val);

	else if (key == "Strength")
		updateSignalStrength(val);

	else if (key == "Technology")
		updateTechnology(val);

	else if (key == "Name")
		updateNetworkName(val);
}


void CellularProvider::updateProperties()
{
	if(!networkProperties || !simProperties) {
		qDebug() << "No Properties found";
		return;
	}

	QVariantMap np = networkProperties->GetProperties();
	QVariantMap sp = simProperties->GetProperties();

	QVariantMap::const_iterator iter = np.begin();
	while (iter != np.end()) {
		updateProperty(iter.key(), QDBusVariant(iter.value()));
		iter++;
	}

	iter = sp.begin();
	while (iter != sp.end()) {
		updateProperty(iter.key(), QDBusVariant(iter.value()));
		iter++;
	}
}

void CellularProvider::updateSimPresent(const QDBusVariant &val)
{
	bool simPresent = val.variant().toBool();
	if (simPresent == FALSE) {
		properties[RegistrationStatus] = QVariant("no-sim");
		emit valueChanged(RegistrationStatus, properties[RegistrationStatus]);

	}
}

void CellularProvider::updateRegistrationStatus(const QDBusVariant &val)
{
	QString status = qdbus_cast<QString> (val.variant());
	if (status == "registered") {
		properties[RegistrationStatus] = QVariant("home");
	}
	else if ( status =="roaming") {
		properties[RegistrationStatus] = QVariant("roam");
	}
	else if (status == "denied") {
		properties[RegistrationStatus] = QVariant("forbidden");
	}
	else {
		properties[RegistrationStatus] =QVariant("offline");
	}
	emit valueChanged(RegistrationStatus, properties[RegistrationStatus]);
}

void CellularProvider::updateCellName(const QDBusVariant &val)
{
	properties[CellName] = val.variant();
	emit valueChanged(CellName, properties[CellName]);
}

void CellularProvider::updateSignalStrength(const QDBusVariant &val)
{
	int strength = val.variant().toInt();

	if(strength >= 0 && strength <= 100) {
		properties[SignalStrength] = QVariant((val.variant()).toInt());
		properties[SignalBars] = QVariant((val.variant()).toInt() / 20); //DUMMY
	} else {
		properties[SignalStrength] = QVariant();
		properties[SignalBars] = QVariant();
	}

	emit valueChanged(SignalStrength, properties[SignalStrength]);
	emit valueChanged(SignalBars, properties[SignalBars]);
}

void CellularProvider::updateTechnology(const QDBusVariant &val)
{
	QString tech = qdbus_cast<QString> (val.variant());

	if (tech == "gsm") {
		properties[DataTechnology] = QVariant("gprs");
		properties[Technology] = QVariant("gsm");
	}
	else if (tech == "edge") {
		properties[DataTechnology] = QVariant("egprs");
		properties[Technology] = QVariant("gsm");
	}
	else if (tech == "umts") {
		properties[DataTechnology] = QVariant("umts");
		properties[Technology] = QVariant("umts");
	}
	else if (tech == "hspa") {
		properties[DataTechnology] = QVariant("hspa");
		properties[Technology] = QVariant("umts");
	}
	else if (tech == "lte") {
		properties[DataTechnology] = QVariant("lte");
		properties[Technology] = QVariant("lte");
	}
	else {
		properties[DataTechnology] = QVariant();
		properties[Technology] = QVariant();

	}
	emit valueChanged(DataTechnology, properties[DataTechnology]);
	emit valueChanged(Technology, properties[Technology]);
}

void CellularProvider::setUnknown(const QString& key) {
	emit valueChanged(key,QVariant());
}

void CellularProvider::updateNetworkName(const QDBusVariant &val)
{
	properties[NetworkName] = val.variant();
	emit valueChanged(NetworkName, properties[NetworkName]);
}

void CellularProvider::emitSubscribeFinished()
{
	foreach(QString key, subscribedProperties)
	{
		emit subscribeFinished(key, properties[key]);
	}
}

