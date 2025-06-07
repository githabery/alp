#include <QCoreApplication>
#include <iostream>
#include "IHttpServer.hpp"
#include "IAudioManager.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    IHttpServer& server = IHttpServer::create();
    IAudioManager& manager = IAudioManager::create();
    manager.loadSettings();

    server.setupRoutes();
    server.start(8080);
    std::cout << "Listening port: " << server.port() << std::endl;

    manager.saveSettings();
    return a.exec();
}
