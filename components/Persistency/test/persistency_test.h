#ifndef PERSISTENCY_TEST_H
#define PERSISTENCY_TEST_H

#include <QObject>

class Persistency_Test : public QObject
{
    Q_OBJECT

private slots:
    void persistence();
};

#endif // PERSISTENCE_TEST_H
