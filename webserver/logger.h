#ifndef LOGGER_H
#define LOGGER_H

#include <QtGlobal>
#include <QMutex>
class QFile;
class QTextStream;

namespace novastory
{

class Logger
{
public:
	explicit Logger();
	~Logger();
	static Logger& Instance()
	{
		static Logger theSingleInstance;
		return theSingleInstance;
	}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
#else
	void messageOutput(QtMsgType type, const char* msg);
#endif

	void log(const QString& text, QtMsgType type = QtMsgType::QtDebugMsg);

	void setWriteToLogFile(bool writToFile);
	void setFailReports(bool enable);

	QByteArray html() const;
private:
	void initializeFileLog();

	QFile* logFile;
	QFile* errorFile;
	QFile* htmlFile;
	QTextStream* logStream;
	QTextStream* errorStream;
	QTextStream* htmlStream;
	bool isWriteToFile;
	QMutex m_mutex;
	bool failReports;
};

}

#endif // LOGGER_H
