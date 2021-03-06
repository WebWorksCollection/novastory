#include "webdatacontainer.h"
#include <QRegExp>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>

void novastory::WebDataContainer::processImage(const QString& data)
{
	QRegExp rx("data:(.*);base64,(.*)");
	if (rx.indexIn(data) >= 0)
	{
		m_mimetype = rx.cap(1);
		QByteArray::operator=(fromBase64(rx.cap(2).toLatin1()));
	}
}

const QString& novastory::WebDataContainer::mimeType() const
{
	return m_mimetype;
}

novastory::WebDataContainer::WebDataContainer(const QString& data) : QByteArray()
{
	processImage(data);
}

novastory::WebDataContainer::WebDataContainer(const QByteArray& other, const QString& mimetype)
	: QByteArray(other), m_mimetype(mimetype)
{

}

novastory::WebDataContainer::WebDataContainer() : QByteArray()
{

}

QString novastory::WebDataContainer::toString() const
{
	return QString("data:%1;base64,%2").arg(m_mimetype).arg(QString::fromLatin1(toBase64()));
}

void novastory::WebDataContainer::setMimeType(const QString& mime)
{
	m_mimetype = mime;
}

QString novastory::WebDataContainer::eTag() const
{
	if (m_modificated.isNull())
	{
		return QString();
	}

	return QString().sprintf("\"%xT-%xO\"", (unsigned int)m_modificated.toMSecsSinceEpoch(), (unsigned int)size());
}

void novastory::WebDataContainer::setRFCData(const QString& rfc2397data)
{
	processImage(rfc2397data);
}

bool novastory::WebDataContainer::detectMimeType()
{
	QMimeDatabase db;
	QMimeType mime = db.mimeTypeForData(*this);
	m_mimetype = mime.name();
	return mime.isValid();
}
