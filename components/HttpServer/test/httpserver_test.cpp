#include <qtcpserver.h>
#include <QCoreApplication>
#include <qtest.h>
#include "httpserver_test.h"
#include "QtHttpServer/QHttpServer"

/****************************************************************************
 * Server tests
 ****************************************************************************/


int HttpServer_Test::server(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    //IHttpServer& server = IHttpServer::create();
    //server.setupRoutes();
    //server.start(55807);
    //while(1);

    QHttpServer server;

    server.route("/", [] () {
        return "hello world";
    });

    auto tcpserver = new QTcpServer();
    if (!tcpserver->listen() || !server.bind(tcpserver)) {
        delete tcpserver;
    }
    qDebug() << "Listening on port" << tcpserver->serverPort();
    return app.exec();
}

QTEST_MAIN(HttpServer_Test)
