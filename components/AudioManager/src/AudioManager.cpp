#include "AudioManager.hpp"
#include <iostream>
#include <qdebug.h>
#include "IBASSAudioReader.hpp"
#include "IPersistency.hpp"

AudioManager AudioManager::sAudioManager;

IAudioManager& IAudioManager::create() {
    return AudioManager::sAudioManager;
}

bool AudioManager::setAsioDevice(int deviceId) {
    IBASSAsioReader& reader = IBASSAsioReader::create();
    bool isSet = reader.setAsioDevice(deviceId);
    if (!isSet)
        return false;

    format = reader.getAsioFormat();
    sampleRate = reader.getSampleRate();

    // Отримуємо імена каналів
    QStringList names = reader.getInputChannelNames();

    // Очищаємо стару конфігурацію
    channelsInfo.clear();

    // Заповнюємо map
    for (int i = 0; i < names.size(); ++i) {
        ChannelInfo info;
        info.name = names[i];
        info.active = false;
        info.DMXChanel = 0;
        info.functions = {};

        channelsInfo[i] = std::move(info);
    }

    return true;
}

bool AudioManager::loadSettings() {
    IPersistency& persistency = IPersistency::create();
    QString sourceDir = "D:/My Documents/Projects/AudioLightProcessor";
    QString filePath = sourceDir + "/components/Persistency/src/Persistency.json";
    persistency.setFilePath(filePath);

    if (!setAsioDevice(deviceId)) {
        qWarning() << "Failed to set ASIO device.";
        return false;
    }

    std::map<int, ChannelInfo> channelsInfo = persistency.getChannelsInfo();
    if (channelsInfo.empty()) {
        qWarning() << "Failed to load channels info.";
        return false;
    }

    for (const auto& [channelId, info] : channelsInfo) {
        if (!info.active)
            continue;

        if (!addChannel(channelId)) {
            qWarning() << "Failed to add channel:" << channelId;
            return false;
        }

        for (AudioFunctionType function : info.functions) {
            if (!addFunctionToChannel(channelId, function)) {
                qWarning() << "Failed to add function to channel:" << channelId << ", function:" << static_cast<int>(function);
                return false;
            }
        }
    }
    IBASSAsioReader& reader = IBASSAsioReader::create();
    if(!reader.isStarted() && hasActiveChannels())
        reader.startAsio();
    return true;
}


bool AudioManager::saveSettings() {
    IPersistency& persistency = IPersistency::create();
    if (!persistency.saveChannelsInfo())
        return false;
    return true;
}

bool AudioManager::addChannel(int channelId) {
    if (channelProcessors.find(channelId) != channelProcessors.end()) {
        std::cerr << "Channel " << channelId << " already added.\n";
        return false;
    }

    IBASSAsioReader& reader = IBASSAsioReader::create();
    if(reader.isStarted())
        reader.stopAsio();

    // Створюємо новий AudioProcessor через інтерфейс
    std::unique_ptr<IAudioProcessor> processor = IAudioProcessor::create();
    processor->init(channelsInfo[channelId].DMXChanel, sampleRate);
    channelProcessors[channelId] = std::move(processor);

    // Підписуємось на аудіодані для цього каналу
    IBASSAsioReader::create().subscribeToAudioData(channelId, [this, channelId](const void* buffer, size_t length) {
        this->processSamples(channelId, buffer, length);
    });

    if(hasActiveChannels())
        reader.startAsio();

    channelsInfo[channelId].active = true;
    return true;
}



std::map<int, ChannelInfo> AudioManager::getChannelsInfo() const {
    return channelsInfo;
}

bool AudioManager::removeChannel(int channelId) {
    IBASSAsioReader& reader = IBASSAsioReader::create();
    if(reader.isStarted())
        reader.stopAsio();
    if (!reader.unsubscribeFromAudioData(channelId)) {
        return false;
    }
    channelProcessors.erase(channelId);
    channelsInfo[channelId].active = false;
    if(hasActiveChannels())
        reader.startAsio();
    return true;
}

bool AudioManager::addFunctionToChannel(int channelId, AudioFunctionType functionType) {
    auto it = channelsInfo.find(channelId);
    if (it == channelsInfo.end()) {
        // Канал з таким ID не знайдено
        return false;
    }
    else if (functionType == AudioFunctionType::COUNT) {
        return false;
    }

    ChannelInfo& info = it->second;

    // Перевіряємо, чи є ця функція в списку, щоб не додавати дублікат
    if (std::find(info.functions.begin(), info.functions.end(), functionType) != info.functions.end()) {
        // Функція вже є – нічого не робимо
        return false;
    }

    // Додаємо функцію
    info.functions.push_back(functionType);
    channelProcessors[channelId]->addFunction(functionType);

    return true; // Функція успішно додана
}

bool AudioManager::hasActiveChannels() const {
    for (auto& [id, info] : channelsInfo) {
        if (info.active)
            return true;
    }
    return false;
}

bool AudioManager::removeFunctionFromChannel(int channelId, AudioFunctionType functionType) {
    auto it = channelsInfo.find(channelId);
    if (it == channelsInfo.end()) {
        // Канал з таким ID не знайдено
        return false;
    }
    else if (functionType == AudioFunctionType::COUNT) {
        return false;
    }

    ChannelInfo& info = it->second;

    auto funcIt = std::find(info.functions.begin(), info.functions.end(), functionType);
    if (funcIt == info.functions.end()) {
        // Функція не знайдена у списку
        return false;
    }

    // Видаляємо функцію
    channelProcessors[channelId]->removeFunction(functionType);
    info.functions.erase(funcIt);
    return true; // Успішно видалено
}


void AudioManager::processSamples(int channelId, const void* samples, size_t count) {
    auto it = channelProcessors.find(channelId);
    if (it != channelProcessors.end()) {
        auto& processor = it->second;

        if (format == 16) {
            const int16_t* intSamples = static_cast<const int16_t*>(samples);
            size_t sampleCount = count / sizeof(int16_t);

            processor->addSamples(intSamples, sampleCount);
        }
        else {
            const int32_t* intSamples = static_cast<const int32_t*>(samples);
            size_t sampleCount = count / sizeof(int32_t);

            processor->addSamples(intSamples, sampleCount);
        }

    }
}

void AudioManager::removeAllChannels() {
    std::vector<int> activeChannels;
    for (const auto& [id, info] : channelsInfo) {
        if (info.active)
            activeChannels.push_back(id);
    }

    for (int id : activeChannels) {
        removeChannel(id);
    }
}

QString AudioManager::functionTypeToString(AudioFunctionType type) const {
    switch (type) {
    case AudioFunctionType::PeakDetector: return "PeakDetector";
    case AudioFunctionType::MusicColoriser: return "MusicColoriser";
    default: return "Unknown";
    }
}

AudioFunctionType AudioManager::functionTypeFromString(const QString& str) const {
    if (str == "PeakDetector")
        return AudioFunctionType::PeakDetector;
    else if (str == "MusicColoriser")
        return AudioFunctionType::MusicColoriser;
    return AudioFunctionType::COUNT;
}

QStringList AudioManager::getFunctionTypes() const {
    QStringList types;
    for (int i = 0; i < AudioFunctionType::COUNT; ++i) {
        types.append(functionTypeToString(static_cast<AudioFunctionType>(i)));
    }
    return types;
}
