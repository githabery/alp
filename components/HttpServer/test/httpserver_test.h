#ifndef HTTPSERVER_TEST_H
#define HTTPSERVER_TEST_H

#include <QObject>

class HttpServer_Test : public QObject
{
    Q_OBJECT

private slots:
    int server(int argc, char *argv[]);
};

#endif // HTTPSERVER_TEST_H
