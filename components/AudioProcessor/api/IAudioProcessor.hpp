#pragma once

#include <cstdint>
#include <memory>
#include "../../AudioManager/api/ChannelInfo.hpp"


class IAudioProcessor {
public:
    virtual void init(int channel, float sampleRate) = 0;

    virtual void addSamples(const int16_t* samples, size_t count) = 0;
    virtual void addSamples(const int32_t* samples, size_t count) = 0;

    virtual bool addFunction(AudioFunctionType type) = 0;

    virtual bool removeFunction(AudioFunctionType type) = 0;

    static std::unique_ptr<IAudioProcessor> create();

};
