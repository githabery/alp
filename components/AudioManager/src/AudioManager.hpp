#pragma once

#include <map>
#include <QStringList>
#include "ChannelInfo.hpp"
#include "IAudioProcessor.hpp"
#include "IAudioManager.hpp"

class AudioManager: public IAudioManager {
public:
    static AudioManager sAudioManager;

    bool addChannel(int channelId) override;
    bool removeChannel(int channelId) override;
    void removeAllChannels() override;
    bool hasActiveChannels() const override;

    bool setAsioDevice(int deviceId) override;

    bool loadSettings() override;
    bool saveSettings() override;

    bool addFunctionToChannel(int channnelId, AudioFunctionType functionType) override;
    bool removeFunctionFromChannel(int channnelId, AudioFunctionType functionType) override;

    QString functionTypeToString(AudioFunctionType type) const override;
    AudioFunctionType functionTypeFromString(const QString& str) const override;

    std::map<int, ChannelInfo> getChannelsInfo() const override;
    QStringList getFunctionTypes() const override;

private:
    void processSamples(int channelId, const void* samples, size_t count);

    AudioManager() = default;
    ~AudioManager() = default;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    int deviceId = 0;
    int format;
    float sampleRate;

    std::map<int, std::unique_ptr<IAudioProcessor>> channelProcessors;
    std::map<int, ChannelInfo> channelsInfo;
};
