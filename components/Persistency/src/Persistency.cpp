#include "Persistency.hpp"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include "IAudioManager.hpp"

Persistency Persistency::sPersistency;

Persistency::Persistency(){}
Persistency::~Persistency() {}

IPersistency& IPersistency::create() {
    return Persistency::sPersistency;
}

bool Persistency::setFilePath(const QString& filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "File does not exist:" << filePath;
        return false;
    }

    this->filePath = filePath;
    return true;
}

bool Persistency::saveChannelsInfo() {
    IAudioManager& manager = IAudioManager::create();
    std::map<int, ChannelInfo> channelsInfo = manager.getChannelsInfo();
    return save(channelsInfo);
}

std::map<int, ChannelInfo> Persistency::getChannelsInfo() {
    return load();
}

bool Persistency::save(const std::map<int, ChannelInfo>& channelsInfo) {
    QJsonObject root;

    // Зберігаєм тільки channelsInfo
    QJsonArray channelsArray;
    for (const auto& [id, info] : channelsInfo) {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = info.name;
        obj["active"] = info.active;
        obj["DMXChannel"] = info.DMXChanel;

        QJsonArray funcs;
        for (const auto& f : info.functions)
            funcs.append(functionTypeToString(f));
        obj["functions"] = funcs;

        channelsArray.append(obj);
    }

    root["channelsInfo"] = channelsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}


std::map<int, ChannelInfo> Persistency::load() {
    std::map<int, ChannelInfo> channelsInfo;
    channelsInfo.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return channelsInfo;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return channelsInfo;

    QJsonObject root = doc.object();



    QJsonArray channelsArray = root["channelsInfo"].toArray();
    for (int i = 0; i < channelsArray.size(); ++i) {
        const QJsonValue& val = channelsArray[i];
        QJsonObject obj = val.toObject();
        ChannelInfo info;
        int id = obj["id"].toInt();
        info.name = obj["name"].toString();
        info.active = obj["active"].toBool();
        info.DMXChanel = obj["DMXChannel"].toInt();

        QJsonArray funcs = obj["functions"].toArray();
        for (int i = 0; i < funcs.size(); ++i) {
            QString funcStr = funcs[i].toString();
            auto funcType = functionTypeFromString(funcStr);
            if (funcType != AudioFunctionType::COUNT) {
                info.functions.push_back(funcType);
            } else {
                qWarning() << "Unknown function type in JSON:" << funcStr;
            }
        }

        channelsInfo[id] = info;
    }

    return channelsInfo;
}



QString Persistency::functionTypeToString(AudioFunctionType type) const {
    switch (type) {
    case AudioFunctionType::PeakDetector: return "PeakDetector";
    case AudioFunctionType::MusicColoriser: return "MusicColoriser";
    default: return "Unknown";
    }
}

AudioFunctionType Persistency::functionTypeFromString(const QString& str) const {
    if (str == "PeakDetector") return AudioFunctionType::PeakDetector;
    else if (str == "MusicColoriser") return AudioFunctionType::MusicColoriser;
    return AudioFunctionType::COUNT;
}
