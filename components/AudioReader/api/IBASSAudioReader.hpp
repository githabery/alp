#pragma once

#include <QStringList>
#include <vector>
#include <string>
#include <functional>

class IBASSAsioReader {
public:

    virtual ~IBASSAsioReader() = default;

    // Получение списка ASIO-устройств
    virtual std::vector<std::string> getAsioDevices() = 0;

    // Получение частоты дискретизации
    virtual float getSampleRate() = 0;
    virtual int getInputChannelCount() const = 0;
    virtual QStringList getInputChannelNames() const = 0;

    // Установка ASIO-устройства
    virtual bool setAsioDevice(int deviceIndex) = 0;

    virtual int getAsioFormat() = 0;

    // Подписка на аудиоданные
    virtual bool subscribeToAudioData(unsigned int asioChannel, std::function<void(const void*, unsigned int)> callback) = 0;

    // Отписка от аудиопотока
    virtual bool unsubscribeFromAudioData(unsigned int asioChannel) = 0;

    virtual bool startAsio() = 0;
    virtual bool stopAsio() = 0;
    virtual bool freeAsio() = 0;

    virtual bool isStarted() const = 0;

    static IBASSAsioReader& create();
};
