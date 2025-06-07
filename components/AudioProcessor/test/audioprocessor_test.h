#ifndef AUDIOPROCESSOR_TEST_H
#define AUDIOPROCESSOR_TEST_H

#include <QObject>

class AudioProcessor_Test : public QObject
{
    Q_OBJECT

private slots:
    void processAudioFFT();
};

#endif // AUDIOPROCESSOR_TEST_H
