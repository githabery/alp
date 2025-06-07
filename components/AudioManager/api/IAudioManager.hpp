#pragma once
#include <map>
#include "ChannelInfo.hpp"

class IAudioManager {
public:
    virtual ~IAudioManager() = default;

    virtual bool addChannel(int channelId) = 0;
    virtual bool removeChannel(int channelId) = 0;
    virtual void removeAllChannels() = 0;
    virtual bool hasActiveChannels() const = 0;

    virtual bool setAsioDevice(int deviceId) =  0;

    virtual bool addFunctionToChannel(int channnelId, AudioFunctionType functionType) = 0;
    virtual bool removeFunctionFromChannel(int channnelId, AudioFunctionType functionType) = 0;

    virtual std::map<int, ChannelInfo> getChannelsInfo() const = 0;
    virtual QStringList getFunctionTypes() const = 0;

    virtual bool loadSettings() = 0;
    virtual bool saveSettings() = 0;

    virtual QString functionTypeToString(AudioFunctionType type) const = 0;
    virtual AudioFunctionType functionTypeFromString(const QString& str) const = 0;

    static IAudioManager& create();
};
