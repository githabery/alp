#include "BASSAsioReader.hpp"
#include <iostream>
#include <QStringList>

BASSAsioReader BASSAsioReader::sBASSAsioReader;

BASSAsioReader::BASSAsioReader() {}

BASSAsioReader::~BASSAsioReader() {
    BASS_ASIO_Stop();
    BASS_ASIO_Free();
}

IBASSAsioReader& IBASSAsioReader::create() {
    return BASSAsioReader::sBASSAsioReader;
}

DWORD CALLBACK BASSAsioReader::AsioProc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user) {
    if (!input) return 0; // Process only input channels

    auto* reader = static_cast<BASSAsioReader*>(user);
    if (reader) {
        // Call audio stream handler
        auto it = reader->audioCallbacks.find(channel);
        if (it != reader->audioCallbacks.end()) {
            it->second(buffer, length);
        }
    }

    return length;
}

std::vector<std::string> BASSAsioReader::getAsioDevices() {
    std::vector<std::string> devices;
    BASS_ASIO_DEVICEINFO info;
    for (int i = 0; BASS_ASIO_GetDeviceInfo(i, &info); ++i) {
        devices.emplace_back(info.name);
        std::cout << "ASIO Device " << i << ": " << info.name << std::endl;
    }
    return devices;
}

int BASSAsioReader::getAsioFormat() {
    DWORD format = BASS_ASIO_ChannelGetFormat(TRUE, 0); // TRUE — вхідний канал 0
    if (format == BASS_ASIO_FORMAT_16BIT)
    {
        return 16;
    }
    else if(format == BASS_ASIO_FORMAT_32BIT)
    {
        return 32;
    }

    return 0;
}

float BASSAsioReader::getSampleRate() {
    return BASS_ASIO_GetRate();
}

int BASSAsioReader::getInputChannelCount() const {
    BASS_ASIO_INFO info = {0};
    if (!BASS_ASIO_GetInfo(&info)) {
        return 0;
    }
    return info.inputs;
}

QStringList BASSAsioReader::getInputChannelNames() const {
    QStringList inputs;
    for (DWORD i = 0; i < getInputChannelCount(); ++i) {
        BASS_ASIO_CHANNELINFO channelInfo;
        if (BASS_ASIO_ChannelGetInfo(TRUE, i, &channelInfo)) {  // TRUE - вхідний канал
            inputs.append(channelInfo.name);
        } else {
            std::cerr << "Failed to get channel info for channel " << i
                      << ": " << BASS_ASIO_ErrorGetCode() << std::endl;
        }
    }
    return inputs;
}

bool BASSAsioReader::setAsioDevice(int deviceIndex) {
    if (!BASS_ASIO_Init(deviceIndex, BASS_ASIO_THREAD)) {
        std::cerr << "ASIO initialization error: " << BASS_ASIO_ErrorGetCode() << std::endl;
        return false;
    }

    if (!BASS_ASIO_SetDevice(deviceIndex)) {
        std::cerr << "ASIO device selection error: " << BASS_ASIO_ErrorGetCode() << std::endl;
        return false;
    }
    return true;
}

bool BASSAsioReader::subscribeToAudioData(unsigned int asioChannel, std::function<void(const void*, unsigned int)> callback) {
    BASS_ASIO_ChannelReset(TRUE, asioChannel, 0);

    if (!BASS_ASIO_ChannelEnable(TRUE, asioChannel, AsioProc, this)) {
        std::cerr << "Failed to enable ASIO channel!" << std::endl;
        return false;
    }

    audioCallbacks[asioChannel] = std::move(callback);

    return true;
}


bool BASSAsioReader::unsubscribeFromAudioData(unsigned int asioChannel) {
    if (audioCallbacks.erase(asioChannel) == 0) {
        std::cerr << "Error: audio channel subscriber not found!" << std::endl;
        return false;
    }

    if (!BASS_ASIO_ChannelReset(TRUE, asioChannel, 0)) {
        std::cerr << "Failed to reset ASIO channel!" << std::endl;
    }

    if (!BASS_ASIO_ChannelEnable(TRUE, asioChannel, NULL, NULL)) {
        std::cerr << "Failed to disable ASIO channel!" << std::endl;
        return false;
    }

    return true;
}

bool BASSAsioReader::startAsio() {
    if (!BASS_ASIO_Start(0, 0)) {
        std::cerr << "Failed to start ASIO!" << BASS_ASIO_ErrorGetCode() << std::endl;
        return false;
    }
    return true;
}

bool BASSAsioReader::isStarted() const {
    return BASS_ASIO_IsStarted();
}

bool BASSAsioReader::stopAsio() {
    bool stopped = BASS_ASIO_Stop();
    if (!stopped)
        std::cerr << "Failed to stop ASIO: " << BASS_ASIO_ErrorGetCode() << std::endl;

    return stopped ;
}

bool BASSAsioReader::freeAsio() {
    bool freed = BASS_ASIO_Free();
    if (!freed)
        std::cerr << "Failed to free ASIO: " << BASS_ASIO_ErrorGetCode() << std::endl;

    return freed;
}
