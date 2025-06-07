#pragma once

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QHttpServer>
#include "IHttpServer.hpp"

class HttpServer : public QObject, public IHttpServer
{
public:
    static HttpServer sHttpServer;

    void setupRoutes() override;
    bool start(quint16 port = 0) override;
    quint16 port() const override;

private:
    QHttpServer m_server;
    quint16 m_port = 0;
};
