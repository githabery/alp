#include <QtTest/QTest>
#include <thread>
#include <iostream>
#include <vector>
#include <cmath>
#include <mutex>

#define private public
#include "audioprocessor_test.h"
#include "IAudioProcessor.hpp"
#include "IBASSAudioReader.hpp"
#include "ChannelInfo.hpp"
#undef private

/****************************************************************************
 * Asio tests
 ****************************************************************************/

//IAudioProcessor& processor = IAudioProcessor::create(); // глобально, чтобы доступ был в callback

std::vector<float> audioBuffer;
std::mutex bufferMutex; // если потоково, лучше защититься
float smoothedRMS = 0.0;


void applyHannWindow(std::vector<float>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (data.size() - 1)));
        data[i] *= multiplier;
    }
}

void processAudio(const void* buffer, unsigned int length) {
    std::unique_ptr<IAudioProcessor> processor = IAudioProcessor::create();
    processor->addFunction(AudioFunctionType::PeakDetector);
    const int32_t* samples = static_cast<const int32_t*>(buffer);
    std::vector<float>inputBuffer;
    for (size_t i = 0; i < length; ++i) {
        float sample = static_cast<float>(samples[i]) / 2147483647.0f;
        sample *= 15;
        inputBuffer.push_back(sample);
    }

    float rms = 0.0f;
    for (float sample : inputBuffer)
        rms += sample * sample;
    rms = std::sqrt(rms / inputBuffer.size());
    const float alpha = 0.2f; //0.1-0.3
    smoothedRMS = (1.0f - alpha) * smoothedRMS + alpha * rms;

    if (smoothedRMS > 0.02f)
        std::cout<< "Ты что то сказал!";
}


void AudioProcessor_Test::processAudioFFT()
{
    IBASSAsioReader& reader = IBASSAsioReader::create();
    reader.getAsioDevices();
    if (!reader.setAsioDevice(0)) {
        std::cerr << "Failed to select ASIO device!" << std::endl;
        return;
    }

    if (!reader.subscribeToAudioData(0, processAudio)) {
        std::cerr << "Failed to subscribe to audio stream!" << std::endl;
        return;
    }

    std::cout << "Started! Waiting for data..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    reader.unsubscribeFromAudioData(0);
    reader.stopAsio();
}

QTEST_MAIN(AudioProcessor_Test)
