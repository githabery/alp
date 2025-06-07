#include "AudioProcessor.hpp"
#include "IDMX512Delivery.hpp"
#include <cmath>
#include <iostream>

AudioProcessor::AudioProcessor()
    : inputBuffer(), fftOutput(bufferSize / 2 + 1), spectrum(bufferSize / 2 + 1), bufferFull(false)
{
    inputBuffer.reserve(bufferSize);
    fftCfg = kiss_fftr_alloc(bufferSize, 0, nullptr, nullptr);
    for (int i = 0; i < AudioFunctionType::COUNT; ++i) {
        enabledFunctions[AudioFunctionType(i)] = false;
    }
}

AudioProcessor::~AudioProcessor() {
    if (fftCfg)
        free(fftCfg);
}

std::unique_ptr<IAudioProcessor> IAudioProcessor::create() {
    return std::make_unique<AudioProcessor>();
}

void AudioProcessor::init(int channel, float sampleRate) {
    this->channel = channel;
    this->sampleRate = sampleRate;
}

void AudioProcessor::addSamples(const int16_t* samples, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        float sample = static_cast<float>(samples[i]) / 32767.0f;
        sample *= 15;
        inputBuffer.push_back(sample);

        if (inputBuffer.size() >= bufferSize) {
            bufferFull = true;
            if (enabledFunctions[AudioFunctionType::MusicColoriser]) {
                musicColoriser();
            }
        }
    }

    if (enabledFunctions[AudioFunctionType::PeakDetector]) {
        peakDetector();
    }
}

void AudioProcessor::addSamples(const int32_t* samples, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        float sample = static_cast<float>(samples[i]) / 2147483647.0f;
        sample *= 15;
        inputBuffer.push_back(sample);

        if (inputBuffer.size() >= bufferSize) {
            bufferFull = true;
            if (enabledFunctions[AudioFunctionType::MusicColoriser]) {
                musicColoriser();
            }
        }
    }

    if (enabledFunctions[AudioFunctionType::PeakDetector]) {
        peakDetector();
    }
}

bool AudioProcessor::addFunction(AudioFunctionType type) {
    auto it = enabledFunctions.find(type);
    if (it != enabledFunctions.end()) {
        it->second = true;
        return true;
    }
    return false;
}

bool AudioProcessor::removeFunction(AudioFunctionType type) {
    auto it = enabledFunctions.find(type);
    if (it != enabledFunctions.end()) {
        it->second = false;
        return true;
    }
    return false;
}



void AudioProcessor::printSpectrum() const {
    const size_t barCount = 16;
    size_t spectrumSize = spectrum.size();
    size_t segmentSize = spectrumSize / barCount;

    if (segmentSize == 0) {
        std::cout << "Spectrum too small to print bars." << std::endl;
        return;
    }

    std::vector<float> averages(barCount, 0.0f);

    // Середнє арифметичне у кожному сегменті
    for (size_t i = 0; i < barCount; ++i) {
        size_t start = i * segmentSize;
        size_t end = (i == barCount - 1) ? spectrumSize : (start + segmentSize);

        float sum = 0.0f;
        for (size_t j = start; j < end; ++j) {
            sum += spectrum[j];
        }
        averages[i] = sum / (end - start);
    }

    constexpr float gain = 50.0f;  // посилення для зручного відображення

    std::cout << "Spectrum (16 bars):\n";

    for (size_t i = 0; i < barCount; ++i) {
        int barHeight = static_cast<int>(averages[i] * gain);
        if (barHeight > 10) barHeight = 10;  // максимум 10 грат

        for (int h = 0; h < barHeight; ++h) {
            std::cout << '#';
        }
        if (barHeight == 0) std::cout << '.';
        std::cout << std::endl;
    }
}


void AudioProcessor::applyHannWindow(std::vector<float>& buffer) {
    for (size_t i = 0; i < buffer.size(); ++i) {
        float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (buffer.size() - 1)));
        buffer[i] *= multiplier;
    }
}

void AudioProcessor::computeFFT() {
    if (!bufferFull) return;

    applyHannWindow(inputBuffer);
    kiss_fftr(fftCfg, inputBuffer.data(), fftOutput.data());

    for (size_t i = 0; i < fftOutput.size(); ++i) {
        float mag = std::sqrt(fftOutput[i].r * fftOutput[i].r + fftOutput[i].i * fftOutput[i].i);
        spectrum[i] = mag;
    }

    inputBuffer.clear();
    bufferFull = false;
}

void AudioProcessor::peakDetector() {
    IDMX512Delivery& dmxDelivery = IDMX512Delivery::instance();

    float rms = 0.0f;
    for (float sample : inputBuffer)
        rms += sample * sample;
    rms = std::sqrt(rms / inputBuffer.size());

    float alpha = 0.3f;

    smoothedRMS = (1.0f - alpha) * smoothedRMS + alpha * rms;

    if (smoothedRMS > 0.015f && isRising == false) {
        isRising = true;
        if (dmxDelivery.isPulseReady())
            dmxDelivery.pulse(channel);
        std::cout<< "Send Pulse" << std::endl;
    }
    else if (smoothedRMS <= 0.015f && isRising == true) {
        isRising = false;
    }
    if (bufferFull) {
        inputBuffer.clear();
        bufferFull = false;
    }
}

void AudioProcessor::musicColoriser() {
    IDMX512Delivery& dmxDelivery = IDMX512Delivery::instance();

    computeFFT();
    float frequency = getDominantFrequency();

    if (frequency > 8000.0f) {
        frequency = 8000.0f;
    }

    int r, g, b;

    if (frequency < 80.0f) {
        // Нижче 80 Гц — темно-червоний
        r = 128;
        g = 0;
        b = 0;
    }
    else if (frequency <= 4200.0f) {
        // Основний діапазон інструментів 80–4200 Гц
        // Використовуємо логарифмічний розподіл для більшої щільності на низьких частотах

        float logFreq = log(frequency / 80.0f) / log(4200.0f / 80.0f); // 0.0 - 1.0

        if (logFreq <= 0.2f) {
            // 80–200 Гц: Темно-червоний → Червоний
            float t = logFreq / 0.2f;
            r = (int)(128 + t * 127);
            g = 0;
            b = 0;
        }
        else if (logFreq <= 0.35f) {
            // 200–500 Гц: Червоний → Помаранчевий
            float t = (logFreq - 0.2f) / 0.15f;
            r = 255;
            g = (int)(t * 165);
            b = 0;
        }
        else if (logFreq <= 0.5f) {
            // 500–900 Гц: Помаранчевий → Жовтий
            float t = (logFreq - 0.35f) / 0.15f;
            r = 255;
            g = (int)(165 + t * 90);
            b = 0;
        }
        else if (logFreq <= 0.65f) {
            // 900–1600 Гц: Жовтий → Жовто-зелений
            float t = (logFreq - 0.5f) / 0.15f;
            r = (int)(255 - t * 128);
            g = 255;
            b = 0;
        }
        else if (logFreq <= 0.8f) {
            // 1600–2500 Гц: Жовто-зелений → Зелений
            float t = (logFreq - 0.65f) / 0.15f;
            r = (int)(127 - t * 127);
            g = 255;
            b = 0;
        }
        else {
            // 2500–4200 Гц: Зелений → Блакитний
            float t = (logFreq - 0.8f) / 0.2f;
            r = 0;
            g = 255;
            b = (int)(t * 255);
        }
    }
    else if (frequency <= 6000.0f) {
        // 4200–6000 Гц: Блакитний → Синій
        float t = (frequency - 4200.0f) / (6000.0f - 4200.0f);
        r = 0;
        g = (int)(255 - t * 255);
        b = 255;
    }
    else {
        // 6000–8000 Гц: Синій → Фіолетовий
        float t = (frequency - 6000.0f) / (8000.0f - 6000.0f);
        r = (int)(t * 255);
        g = 0;
        b = 255;
    }
    dmxDelivery.setColor(channel, r, g, b);
}

float AudioProcessor::getDominantFrequency() const {
    if (spectrum.empty()) {
        return 0.0f; // Спектр пустой
    }

    // Находимо індекс з максимальною амплітудою
    size_t maxIndex = 0;
    float maxMagnitude = spectrum[0];

    // Починаємо з індексу 1, оскільки індекс 0 — це DC-компонента (0 Гц)
    for (size_t i = 1; i < spectrum.size(); ++i) {
        if (spectrum[i] > maxMagnitude) {
            maxMagnitude = spectrum[i];
            maxIndex = i;
        }
    }

    // Перетворюємо індекс у частоту
    float frequency = (static_cast<float>(maxIndex) * sampleRate) / static_cast<float>(bufferSize);
    //std::cout << "Freq = " << frequency << std::endl;
    return frequency;
}

const std::vector<float>& AudioProcessor::getSpectrum() const {
    return spectrum;
}
