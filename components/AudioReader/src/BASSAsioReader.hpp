#pragma once

#include <bassasio.h>
#include <bass.h>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "IBASSAudioReader.hpp"
#include <QStringList>

class BASSAsioReader : public IBASSAsioReader{
public:
    static BASSAsioReader sBASSAsioReader;

    BASSAsioReader();
    ~BASSAsioReader();

    // Отримання списку ASIO-пристроїв
    std::vector<std::string> getAsioDevices() override;

    // Отримання частоти дискретизації
    float getSampleRate() override;
    int getInputChannelCount() const override;
    QStringList getInputChannelNames() const override;

    // Встановлення ASIO-драйвера
    bool setAsioDevice(int deviceIndex) override;

    // Підписка на аудіодані
    bool subscribeToAudioData(unsigned int asioChannel, std::function<void(const void*, unsigned int)> callback) override;

    // Відписка від аудіопотоку
    bool unsubscribeFromAudioData(unsigned int asioChannel) override;

    bool startAsio() override;
    bool stopAsio() override;
    bool freeAsio() override;

    bool isStarted() const override;

    int getAsioFormat() override;

private:
    static DWORD CALLBACK AsioProc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user);

    std::unordered_map<int, std::function<void(const void*, DWORD)>> audioCallbacks;
};
