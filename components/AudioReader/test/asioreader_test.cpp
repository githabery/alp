#include <QtTest/QTest>
#include <bassasio.h>
#include <thread>

#define private public
#include "asioreader_test.h"
#include "iostream"
#include "../api/IBASSAudioReader.hpp"
#undef private

/****************************************************************************
 * Asio tests
 ****************************************************************************/

void processAudio(const void* buffer, unsigned int length) {
    const int32_t* samples = static_cast<const int32_t*>(buffer);
    unsigned int sampleCount = length / sizeof(int32_t);
    //std::cout << "length: " << length << std::endl;
    for (unsigned int i = 0; i < sampleCount; ++i) {
        short sample = samples[i];
        //std::cout << i << ": " << sample << " " << std::endl;
    }
}

void AsioReader_Test::subscribeAsio()
{
    IBASSAsioReader& reader = IBASSAsioReader::create();
    reader.getAsioDevices();
    if (!reader.setAsioDevice(0)) {
        std::cerr << "Failed to select ASIO device!" << std::endl;
    }

    if (!reader.subscribeToAudioData(0, processAudio)) {
        std::cerr << "Failed to subscribe to audio stream!" << std::endl;
    }

    std::cout << "Started! Waiting for data..." << std::endl;

    /* Wait loop (you can add exit conditions here)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Just wait
    }
    */
    DWORD format = BASS_ASIO_ChannelGetFormat(TRUE, 0); // TRUE — входной канал 0

    if (format == (DWORD)-1) {
        std::cerr << "Error getting channel format: " << BASS_ASIO_ErrorGetCode() << std::endl;
    } else {
        switch(format) {
        case BASS_ASIO_FORMAT_16BIT:
            std::cout << "Format: 16-bit integer" << std::endl;
            break;
        case BASS_ASIO_FORMAT_24BIT:
            std::cout << "Format: 24-bit integer" << std::endl;
            break;
        case BASS_ASIO_FORMAT_32BIT:
            std::cout << "Format: 32-bit integer" << std::endl;
            break;
        case BASS_ASIO_FORMAT_FLOAT:
            std::cout << "Format: 32-bit float" << std::endl;
            break;
        case BASS_ASIO_FORMAT_DSD_LSB:
            std::cout << "Format: DSD LSB" << std::endl;
            break;
        case BASS_ASIO_FORMAT_DSD_MSB:
            std::cout << "Format: DSD MSB" << std::endl;
            break;
        default:
            std::cout << "Unknown format: " << format << std::endl;
            break;
        }
    }

    for(int i = 0; i < reader.getInputChannelCount(); i++)
        qDebug() << reader.getInputChannelNames()[i];

    double sampleRate = BASS_ASIO_GetRate();
    if (sampleRate == -1) {
        std::cerr << "Failed to get ASIO sample rate. Error code: "
                  << BASS_ASIO_ErrorGetCode() << std::endl;
    } else {
        std::cout << "ASIO sample rate: " << sampleRate << " Hz" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    reader.unsubscribeFromAudioData(0);
    //reader.stopAsio();

}

QTEST_MAIN(AsioReader_Test)
