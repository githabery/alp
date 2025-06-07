#pragma once

#include <map>
#include "../../AudioManager/api/ChannelInfo.hpp"

class IPersistency {
public:

    virtual ~IPersistency() = default;

    virtual bool setFilePath(const QString& filePath) = 0;

    virtual bool saveChannelsInfo() = 0;

    virtual std::map<int, ChannelInfo> getChannelsInfo() = 0;

    static IPersistency& create();

};
