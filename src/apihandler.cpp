#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>

namespace novastory
{

ApiHandler::ApiHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{

}

ApiHandler::~ApiHandler()
{

}

bool ApiHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& /* get = "" */)
{
	if (path != "/api")
	{
		return false;
	}

	qDebug() << "API protocol handled";

	QJsonDocument version;
	QJsonObject versionObject;
	versionObject.insert("version", QJsonValue(QString(GIT_REVISION).toInt()));

	version.setObject(versionObject);

	socket->write(version.toJson());

	return true;
}

}
