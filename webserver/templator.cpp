#include <QFile>
#include "version.h"
#include "config.h"
#include "templator.h"
#include "webserver.h"

namespace novastory
{

QMutex Templator::loadTemplateMutex;

QByteArray Templator::generate(
	const QString& title /*= "Novastory"*/,
	const QString& article /*= QString()*/,
	const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/,
	const QString& description /*= QString()*/,
	const QString& keywords /*= QString()*/
)
{
	static QString templateData;
	QString generatedTemplate;

	loadTemplateMutex.lock();
#ifndef QT_DEBUG
	if (templateData.isEmpty())
	{
#endif
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/index.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			loadTemplateMutex.unlock();
			return QByteArray();
		}

		templateData = templateFile.readAll();
#ifndef QT_DEBUG
	}
#endif
	loadTemplateMutex.unlock();


	//replace data
	generatedTemplate = templateData;
	generatedTemplate = generatedTemplate.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{description}", description);
	generatedTemplate = generatedTemplate.replace("{keywords}", keywords);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2015 &copy; Copyright <a href=\"/about\">" PROJECT_NAME " Engine " GIT_DESCRIBE " [r" GIT_REVISION "]</a>");
	QString pref_lang = WebServer::Instance().defaultLanguage();
	if (!pref_lang.isEmpty())
	{
		QRegExp reg = QRegExp("(\\/static\\/[^'\"]+(?:\\.js|\\.css))");
		reg.setMinimal(true);
		generatedTemplate = generatedTemplate.replace(reg, "\\1?lang=" + pref_lang);
	}
	generatedTemplate = generatedTemplate.replace("{user}", "{}");

	// Google Api
	generatedTemplate = generatedTemplate.replace("{google_client_id}", GOOGLE_WEB_CLIENT_ID);
	generatedTemplate = generatedTemplate.replace("{google_client_secret}", GOOGLE_WEB_CLIENT_SECRET);

	QHashIterator<QString, QString> it(add_map);
	while (it.hasNext())
	{
		it.next();
		generatedTemplate = generatedTemplate.replace("{" + it.key() + "}", it.value());
	}

	translate(generatedTemplate);

	qDebug() << "Html template generated with title:" << title;

	return generatedTemplate.toUtf8();
}

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
QByteArray Templator::generate(
	const User& user,
	const QString& title /*= "Novastory"*/,
	const QString& article /*= QString() */,
	const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/,
	const QString& description /*= QString()*/,
	const QString& keywords /*= QString()*/
)
{
	if (user.isLogined())
	{
		return generateLogined(user, title, article, add_map, description, keywords);
	}
	else
	{
		return generate(title, article, add_map, description, keywords);
	}
}

QByteArray Templator::generateLogined(
	const User& user,
	const QString& title /*= "Novastory"*/,
	const QString& article /*= QString() */,
	const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/,
	const QString& description /*= QString()*/,
	const QString& keywords /*= QString()*/
)
{
	static QString templateData;
	QString generatedTemplate;

	loadTemplateMutex.lock();
#ifndef QT_DEBUG
	if (templateData.isEmpty())
	{
#endif
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/index.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			loadTemplateMutex.unlock();
			return QByteArray();
		}

		templateData = templateFile.readAll();
#ifndef QT_DEBUG
	}
#endif
	loadTemplateMutex.unlock();


	//replace data
	generatedTemplate = templateData;
	generatedTemplate = generatedTemplate.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{description}", description);
	generatedTemplate = generatedTemplate.replace("{keywords}", keywords);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2015 &copy; Copyright <a href=\"/about\">" PROJECT_NAME " Engine " GIT_DESCRIBE " [r" GIT_REVISION "]</a>");
	QString pref_lang = WebServer::Instance().defaultLanguage();
	if (!pref_lang.isEmpty())
	{
		QRegExp reg = QRegExp("(\\/static\\/[^'\"]+(?:\\.js|\\.css))");
		reg.setMinimal(true);
		generatedTemplate = generatedTemplate.replace(reg, "\\1?lang=" + pref_lang);
	}
	QString search = "<script id=\"server-data\">";
	int serchIndex = generatedTemplate.indexOf(search);
	if (serchIndex >= 0)
	{
		QJsonObject userObject = user.jsonObject();
		userObject.insert("token", user.token());
		QJsonDocument doc;
		doc.setObject(userObject);
		generatedTemplate = generatedTemplate.insert(serchIndex + search.length(), "USER=" + doc.toJson(QJsonDocument::Compact) + ";");
	}
#ifdef NOVASTORY_BUILD
	generatedTemplate = generatedTemplate.replace("{users.namemail}", !user.firstName().isEmpty() ? user.firstName() : user.email());
#endif

	// Google Api
	generatedTemplate = generatedTemplate.replace("{google_client_id}", GOOGLE_WEB_CLIENT_ID);
	generatedTemplate = generatedTemplate.replace("{google_client_secret}", GOOGLE_WEB_CLIENT_SECRET);

	QHashIterator<QString, QString> it(add_map);
	while (it.hasNext())
	{
		it.next();
		generatedTemplate = generatedTemplate.replace("{" + it.key() + "}", it.value());
	}

	translate(generatedTemplate);

	qDebug() << "Html template generated with title:" << title;

	return generatedTemplate.toUtf8();
}

#endif

}

