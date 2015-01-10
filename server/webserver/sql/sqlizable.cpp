#include "sqlizable.h"
#include "sqlquery.h"
#include "globals.h"
#include <QSqlRecord>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QList>
#include <QDebug>


namespace novastory
{
Sqlizable::Sqlizable() : QObject()
{
	setObjectName("SQLizable_BASE");
}

// Nothing is copying
Sqlizable::Sqlizable(const novastory::Sqlizable& obj)  : QObject()
{
	setObjectName(obj.objectName());

	QVariant auto_increment = obj.property("auto_increment");
	if (auto_increment.isValid())
	{
		setProperty("auto_increment", auto_increment);
	}

	QVariant primary_key = obj.property("primary_key");
	if (primary_key.isValid())
	{
		setProperty("primary_key", primary_key);
	}

	QVariant hidden = obj.property("hidden");
	if (hidden.isValid())
	{
		setProperty("hidden", hidden);
	}
}

// Nothing is copying
Sqlizable& Sqlizable::operator=(const Sqlizable& obj)
{
	setObjectName(obj.objectName());

	QVariant auto_increment = obj.property("auto_increment");
	if (auto_increment.isValid())
	{
		setProperty("auto_increment", auto_increment);
	}

	QVariant primary_key = obj.property("primary_key");
	if (primary_key.isValid())
	{
		setProperty("primary_key", primary_key);
	}

	QVariant hidden = obj.property("hidden");
	if (hidden.isValid())
	{
		setProperty("hidden", hidden);
	}

	return *this;
}


/*
bool skipCleanVariant(const QVariant& variant)
{
	QVariant::Type type = variant.type();

}
*/

bool Sqlizable::insertSQL(bool can_dublicate /*= false*/, bool not_ignore_id /* = false */)
{
	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();
	if (propCount > 0)
	{
		QList<QVariant> values;
		QList<QString> keys;

		QString objName = objectName();
		if (objName.isEmpty())
		{
			return false; // No object name, we doesn't know name of table
		}

		QVariant auto_increment = property("auto_increment");
		bool isAutoIncrement = auto_increment.isValid();

		QString sql = "INSERT INTO `" + objName + "`(";

		for (int i = 0; i < propCount; ++i)
		{
			QMetaProperty prop = mObject->property(i);

			if (isObjectType(prop.userType()))
			{
				continue;
			}

			const QString& propName = prop.name();
			QVariant propValue = prop.read(this);

			// Skip objectName
			if (propName == "objectName")
			{
				continue;
			}

			// Skip auto_increment value
			if (isAutoIncrement && (!not_ignore_id || propValue.toInt() <= 0))
			{
				if (propName == auto_increment.toString())
				{
					continue;
				}
			}

			values.append(propValue);
			keys.append(propName);

			sql += "`" + propName + "`, ";
		}

		if (values.size() > 0)
		{
			sql = sql.left(sql.length() - 2);
		}

		int valuesCout = values.count();

		sql += ") VALUES(";
		for (int i = 0; i < valuesCout; ++i)
		{
			sql += (i == valuesCout - 1) ? ":" + keys[i] : ":" + keys[i] + ", ";
		}
		sql += ")";

		if (can_dublicate)
		{
			sql += " ON DUPLICATE KEY UPDATE ";
			for (int i = 0; i < valuesCout; ++i)
			{
				if (not_ignore_id && keys[i] == property("auto_increment").toString())
				{
					continue;
				}
				sql += (i == valuesCout - 1) ? "`" + keys[i] + "` = :" + keys[i] : "`" + keys[i] + "` = :" + keys[i] + ", ";
			}
		}

		SqlQuery query;
		query.prepare(sql);

		for (int i = 0; i < valuesCout; ++i)
		{
			query.bindValue(":" + keys.at(i), values.at(i));
		}

		bool result = query.exec();

		// Auto increment update proprerty
		if (result && isAutoIncrement)
		{
			setProperty(auto_increment.toString().toUtf8().data(), query.lastInsertId());
		}


		return result;
	}

	return true;
}

bool Sqlizable::syncSQL(const QList<QString>& basis)
{
	QString objName = objectName();
	if (objName.isEmpty())
	{
		return false; // No object name, we doesn't know name of table
	}

	SqlQuery query;

	QString sql = QString("SELECT * FROM `") + objName + "` WHERE ";

	// Founding keys
	QList<QVariant> internalValues;
	/*
	if(property("primary_key").isValid() && property(property("primary_key").toString().toUtf8()).isValid())
	{
		sql += "`" + property("primary_key").toString() + "` = ?";
		internalValues.append(property(property("primary_key").toString().toUtf8()).toString());
	}
	else
	{
		const QMetaObject* mObject = metaObject();
		const int propCount = mObject->propertyCount();
		for (int i = 0; i < propCount; ++i)
		{
			QMetaProperty prop = mObject->property(i);
			const QString& propName = prop.name();
			QVariant propValue = prop.read(this);

			if(!propValue.isValid())
			{
				continue;
			}

			// Skip objectName
			if (propName == "objectName")
			{
				continue;
			}

			sql += (internalValues.size() == 0) ? propName + " = ?" : " AND " + propName + " = ?";
			internalValues.append(propValue);
		}
	}
	*/
	for (QString questVar : basis)
	{
		QVariant propValue = property(questVar.toUtf8());
		sql += (internalValues.size() == 0) ? questVar + " = ?" : " AND " + questVar + " = ?";
		internalValues.append(propValue);
	}

	VERIFY(query.prepare(sql));

	for (int i = 0; i < internalValues.size(); ++i)
	{
		query.bindValue(i, internalValues[i]);
	}

	if (!query.exec())
	{
		qWarning() << "No sync record from" << objName << "founded";
		return false;
	}

	return syncProcess(query);
}


bool Sqlizable::syncProcess(SqlQuery& query)
{
	if (query.size() != 1)
	{
		qDebug() << "Not only one record in database. Can not sync.";
		return false;
	}

	VERIFY(query.next());
	syncRecord(query);

	return true;
}



void Sqlizable::syncRecord(SqlQuery& query)
{
	QSqlRecord record = query.record();
	syncRecord(record);
}

void Sqlizable::syncRecord(QSqlRecord& record)
{
	for (int i = 0; i < record.count(); ++i)
	{
		QString propertyName = record.fieldName(i);
		QVariant propertyValue = record.value(i);
		setProperty(propertyName.toLatin1(), propertyValue);
		qDebug() << "SYNCSQL" << propertyName << "=" << propertyValue.toString();
	}
}


bool Sqlizable::syncSQL(const QString& basis)
{
	return syncSQL(QList<QString>() << basis);
}

bool Sqlizable::removeSQL(const QList<QString>& basis)
{
	QString objName = objectName();
	if (objName.isEmpty())
	{
		return false; // No object name, we doesn't know name of table
	}

	SqlQuery query;

	QString sql = QString("DELETE FROM `") + objName + "` WHERE ";

	// Founding keys
	QList<QVariant> internalValues;

	for (QString questVar : basis)
	{
		QVariant propValue = property(questVar.toUtf8());
		sql += (internalValues.size() == 0) ? questVar + " = ?" : " AND " + questVar + " = ?";
		internalValues.append(propValue);
	}

	VERIFY(query.prepare(sql));

	for (int i = 0; i < internalValues.size(); ++i)
	{
		query.bindValue(i, internalValues[i]);
	}

	if (!query.exec())
	{
		qWarning() << "Removing " << objName << " failed";
		return false;
	}

	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();
	for (int i = 0; i < propCount; ++i)
	{
		QMetaProperty prop = mObject->property(i);
		prop.reset(this);
	}

	return true;
}

bool Sqlizable::removeSQL(const QString& basis)
{
	return removeSQL(QList<QString>() << basis);
}

bool Sqlizable::updateSQL(const QList<QString>& basis, const QList<QString>& ignoreVariables /* = QList<QString>() */)
{
	QString objName = objectName();
	if (objName.isEmpty())
	{
		return false; // No object name, we doesn't know name of table
	}

	SqlQuery query;

	QString sql = QString("UPDATE `") + objName + "` SET ";

	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();
	if (propCount == 0)
	{
		return false;
	}

	QVariant auto_increment = property("auto_increment");
	bool isAutoIncrement = auto_increment.isValid();

	QList<QVariant> values;

	for (int i = 0; i < propCount; ++i)
	{
		QMetaProperty prop = mObject->property(i);

		if (isObjectType(prop.userType()))
		{
			continue;
		}

		const QString& propName = prop.name();
		QVariant propValue = prop.read(this);

		// Skip objectName
		if (propName == "objectName")
		{
			continue;
		}

		// Skip auto_increment value
		if (isAutoIncrement)
		{
			if (propName == auto_increment.toString())
			{
				continue;
			}
		}

		// ignore list
		if (ignoreVariables.indexOf(propName) >= 0)
		{
			continue;
		}

		values.append(propValue);

		sql += "`" + propName + "` = ?, ";
	}

	if (values.size() > 0)
	{
		sql = sql.left(sql.length() - 2) + " WHERE ";
	}

	// Founding keys
	QList<QVariant> internalValues;

	for (QString questVar : basis)
	{
		QVariant propValue = property(questVar.toUtf8());
		sql += (internalValues.size() == 0) ? questVar + " = ?" : " AND " + questVar + " = ?";
		internalValues.append(propValue);
	}

	qDebug() << sql;

	VERIFY(query.prepare(sql));

	for (int i = 0; i < values.count(); ++i)
	{
		query.bindValue(i, values.at(i));
	}

	for (int i = 0, j = values.count(); i < internalValues.size(); ++i, ++j)
	{
		query.bindValue(j, internalValues[i]);
	}

	if (!query.exec())
	{
		qWarning() << "Updating " << objName << " failed";
		return false;
	}

	return true;
}

bool Sqlizable::updateSQL(const QString& basis, const QList<QString>& ignoreVariables /* = QList<QString>() */)
{
	return updateSQL(QList<QString>() << basis, ignoreVariables);
}

QJsonObject Sqlizable::jsonObject() const
{
	QJsonObject obj;

	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();

	QVariant hiddenArray = property("hidden");
	bool isHiddenFields = hiddenArray.isValid();
	QStringList hiddenFields;
	if (isHiddenFields)
	{
		hiddenFields = hiddenArray.toString().split(",");
	}

	for (int i = 0; i < propCount; ++i)
	{
		QMetaProperty prop = mObject->property(i);

		if (isObjectType(prop.userType()))
		{
			continue;
		}

		const QString& propName = prop.name();
		QVariant propValue = prop.read(this);

		// Skip objectName
		if (propName == "objectName")
		{
			continue;
		}

		// ignore list
		if (hiddenFields.indexOf(propName) >= 0)
		{
			continue;
		}

		obj.insert(propName, QJsonValue::fromVariant(property(propName.toUtf8())));
	}

	return obj;
}

void Sqlizable::substitute(QString& data, QString prefix /*= QString()*/) const
{
	QString namesp = objectName();
	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();

	for (int i = 0; i < propCount; ++i)
	{
		QMetaProperty prop = mObject->property(i);

		if (Sqlizable::isObjectType(prop.userType()))
		{
			continue;
		}

		const QString& propName = prop.name();
		QVariant propValue = prop.read(this);

		// Skip objectName
		if (propName == "objectName")
		{
			continue;
		}

		if (prefix.isNull())
		{
			data = data.replace(QString("{%1.%2}").arg(namesp).arg(propName), propValue.toString());
		}
		else
		{
			data = data.replace(QString("{%1.%2.%3}").arg(prefix).arg(namesp).arg(propName), propValue.toString());
		}
	}
}

}

