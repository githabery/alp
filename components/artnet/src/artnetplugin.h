/*
  Q Light Controller Plus
  artnetplugin.h

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef ARTNETPLUGIN_H
#define ARTNETPLUGIN_H

#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QNetworkInterface>
#include <QHostAddress>
#include <QString>
#include <QHash>
#include <QFile>

#include "artnetcontroller.h"
#include "IDMX512Config.hpp"
#include "IDMX512Delivery.hpp"

#define SETTINGS_IFACE_WAIT_TIME "ArtNetPlugin/ifacewait"

typedef struct _aio
{
    QNetworkInterface iface;
    QNetworkAddressEntry address;
    ArtNetController* controller;
} ArtNetIO;

#define ARTNET_INPUTUNI "inputUni"
#define ARTNET_OUTPUTIP "outputIP"
#define ARTNET_OUTPUTUNI "outputUni"
#define ARTNET_TRANSMITMODE "transmitMode"

class ArtNetPlugin: public QObject
                  , public IDMX512Config
                  , public IDMX512Delivery
{
    Q_OBJECT

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:
    /** @reimp */
    virtual ~ArtNetPlugin();

    void init();

    int capabilities() const;

private:
    bool requestLine(quint32 line);

    /*********************************************************************
     * Outputs
     *********************************************************************/
public:
    bool openOutput(quint32 output, quint32 universe);
    void closeOutput(quint32 output, quint32 universe);

    /** @reimp */
    virtual bool openOutput(std::string output, uint32_t universe) override;

    /** @reimp */
    virtual void closeOutput(std::string output, uint32_t universe) override;

    /** @reimp */
    virtual std::vector<std::string> outputs() override;

    void writeUniverse(quint32 universe, quint32 output, const QByteArray& data, bool dataChanged);

    /** @reimp */
    virtual void writeUniverse(uint32_t universe, const QByteArray &data) override;

    /** Get a list of the available Input/Output lines */
    QList<ArtNetIO> getIOMapping();


private:
    /** Map of the ArtNet plugin Input/Output lines */
    QList<ArtNetIO> m_IOmapping;

    /** Time to wait (in seconds) for interfaces to be ready */
    int m_ifaceWaitTime;

    /*********************************************************************
     * ArtNet socket
     *********************************************************************/
private:
    QSharedPointer<QUdpSocket> getUdpSocket();
    void handlePacket(QByteArray const& datagram, QHostAddress const& senderAddress);

private slots:
    void slotReadyRead();

private:
    QWeakPointer<QUdpSocket> m_udpSocket;

public:
    static ArtNetPlugin sArtNetPlugin;
};

#endif
