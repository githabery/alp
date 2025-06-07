/*
  Q Light Controller Plus
  artnetplugin.h

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  http://www.apache.org/licenses/LICENSE-2.0.txt
*/

#ifndef ARTNETPLUGIN_H
#define ARTNETPLUGIN_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QFile>
#include <QList>
#include <QHash>
#include <QByteArray>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QUdpSocket>

#include "artnetcontroller.h"
#include "IDMX512Config.hpp"
#include "IDMX512Delivery.hpp"

#define SETTINGS_IFACE_WAIT_TIME "ArtNetPlugin/ifacewait"
#define ARTNET_INPUTUNI          "inputUni"
#define ARTNET_OUTPUTIP          "outputIP"
#define ARTNET_OUTPUTUNI         "outputUni"
#define ARTNET_TRANSMITMODE      "transmitMode"

struct ArtNetIO
{
    QNetworkInterface iface;
    QNetworkAddressEntry address;
    ArtNetController* controller;
};

class ArtNetPlugin : public QObject,
                     public IDMX512Config,
                     public IDMX512Delivery
{
    Q_OBJECT

public:
    ArtNetPlugin() = default;
    virtual ~ArtNetPlugin();

    // Initialization
    void init();
    int capabilities() const;

    // I/O management
    QList<ArtNetIO> getIOMapping();

    // Output operations
    bool openOutput(quint32 output, quint32 universe);
    void closeOutput(quint32 output, quint32 universe);
    void writeUniverse(quint32 universe, quint32 output, const QByteArray& data, bool dataChanged);

    // IDMX512Delivery interface
    void pulse(int channel) override;
    bool isPulseReady() override;
    void setColor(int channel, int r, int g, int b) override;
    void setBrightness(int channel, int brightness) override;

    // IDMX512Config interface
    bool openOutput(std::string output, uint32_t universe) override;
    void closeOutput(std::string output, uint32_t universe) override;
    std::vector<std::string> outputs() override;

    // Singleton instance
    static ArtNetPlugin sArtNetPlugin;

private:
    // Internal helpers
    bool requestLine(quint32 line);
    void setColorSmooth(int channel, int targetR, int targetG, int targetB);
    void updateChannel(int& current, int target);

    // UDP handling
    QSharedPointer<QUdpSocket> getUdpSocket();
    void handlePacket(const QByteArray& datagram, const QHostAddress& senderAddress);

private slots:
    void slotReadyRead();

private:
    // Internal state
    QList<ArtNetIO> m_IOmapping;
    QWeakPointer<QUdpSocket> m_udpSocket;
    int m_ifaceWaitTime = 0;

    quint32 universe = 0;
    int r = 0, g = 0, b = 0;
    bool pulseReady = true;
};

#endif // ARTNETPLUGIN_H
