#ifndef	TEXTREVISION_H
#define TEXTREVISION_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "user.h"

class Test_TextRevision;

namespace novastory
{

class TextRevisionContainer;

class TextRevision : protected Sqlizable, public JsonThrower
{
	friend class ::Test_TextRevision;
	friend class TextRevisionContainer;
	Q_OBJECT
	Q_PROPERTY(int revisionid READ revisionId WRITE setRevisionID)
	Q_PROPERTY(int userid READ userid WRITE setUserID)
	Q_PROPERTY(bool release READ isRelease WRITE setRelease RESET resetRelease);
	Q_PROPERTY(QString text READ text WRITE setText RESET resetText)
public:
	TextRevision();

	int userid() const;
	void setUserID(int userid);
	void setUser(const User& user);

	const QString& text() const;
	void setText(const QString& text);
	void resetText();

	bool isRelease() const;
	void setRelease(bool rel);
	void resetRelease();

	const int& revisionId() const;
	void setRevisionID(int revision);

	bool operator==(const TextRevision& rv) const;
private:
	User m_user;
	QString m_text;
	int m_revisionId;
	bool m_release;
};

}

#endif
