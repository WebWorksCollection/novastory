#include "webserver.h"
#include <QTcpSocket>
#include "webrouter.h"
#include "utils/globals.h"
#include "sql/sqldatabase.h"

namespace novastory
{

WebServer::WebServer(QObject* parent)
	: QTcpServer(parent)
{
	VERIFY(listen(QHostAddress::Any, 8008));
	VERIFY(connect(this, SIGNAL(newConnection()), this, SLOT(someNewConnection())));
	qDebug() << "Web server started at " << serverAddress() << ":" << serverPort();
	SqlDatabase::Instance(); // first call
}


WebServer::~WebServer()
{

}

void WebServer::someNewConnection()
{
	QTcpSocket* clientSocket = nextPendingConnection();
	qDebug() << "New connection established from " << clientSocket->localAddress();
	VERIFY(connect(clientSocket, SIGNAL(readyRead()), this, SLOT(showHtmlPage())));
}

void WebServer::showHtmlPage()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();

	WebRouter urlRouter(clientSocket);
	urlRouter.parse();
	qDebug() << "path = " << urlRouter.path();
	urlRouter.sendHtml();

	clientSocket->close();
}

}