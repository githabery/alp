#pragma once
#include <QString>

enum AudioFunctionType {
    PeakDetector,
    MusicColoriser,
    COUNT
};

struct ChannelInfo {
    QString name;
    bool active = false;
    int DMXChanel;
    std::vector<AudioFunctionType> functions;
};


