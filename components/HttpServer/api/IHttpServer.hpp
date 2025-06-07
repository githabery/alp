#pragma once
#include <qtypes.h>

class IHttpServer {
public:

    virtual void setupRoutes() = 0;

    virtual bool start(quint16 port) = 0;

    virtual quint16 port() const = 0;

    static IHttpServer& create();
};
