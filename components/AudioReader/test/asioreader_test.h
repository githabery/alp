#ifndef ASIOREADER_TEST_H
#define ASIOREADER_TEST_H

#include <QObject>

class AsioReader_Test : public QObject
{
    Q_OBJECT

private slots:
    void subscribeAsio();
};

#endif // ASIOREADER_TEST_H
