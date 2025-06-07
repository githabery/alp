#pragma once

#include "kiss_fftr.h"
#include <cstdint>
#include <vector>
#include <map>
#include "IAudioProcessor.hpp"

class AudioProcessor: public IAudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();

    void init(int channel, float sampleRate) override;

    void addSamples(const int16_t* samples, size_t count) override;
    void addSamples(const int32_t* samples, size_t count) override;

    bool addFunction(AudioFunctionType type) override;
    bool removeFunction(AudioFunctionType type) override;

    const std::vector<float>& getSpectrum() const;

private:
    static constexpr size_t bufferSize = 1024;

    float getDominantFrequency() const;

    void applyHannWindow(std::vector<float>& buffer);

    void printSpectrum() const;

    void peakDetector();
    void musicColoriser();

    void computeFFT();

    std::vector<float> inputBuffer;
    kiss_fftr_cfg fftCfg;
    std::vector<kiss_fft_cpx> fftOutput;
    std::vector<float> spectrum;
    std::map<AudioFunctionType, bool> enabledFunctions;
    float smoothedRMS = 0.0f;
    float sampleRate = 44100.0f;

    int channel;

    bool bufferFull;
    bool isRising = false;
};
