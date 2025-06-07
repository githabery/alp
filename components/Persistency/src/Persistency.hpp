#pragma once

#include <map>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include "IPersistency.hpp"
#include "ChannelInfo.hpp"

class Persistency: public IPersistency{
public:
    Persistency();
    ~Persistency();

    static Persistency sPersistency;

    bool setFilePath(const QString& filePath) override;

    bool saveChannelsInfo() override;

    std::map<int, ChannelInfo> getChannelsInfo() override;




private:
    QString filePath;

    bool save(const std::map<int, ChannelInfo>& channelsInfo);
    std::map<int, ChannelInfo> load();

    QString functionTypeToString(AudioFunctionType type) const;
    AudioFunctionType functionTypeFromString(const QString& str) const;
};
