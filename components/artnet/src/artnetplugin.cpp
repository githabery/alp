/*
  Q Light Controller Plus
  artnetplugin.cpp

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

#include <QSettings>
#include <QDebug>
#include <thread>

#include "artnetplugin.h"

bool addressCompare(const ArtNetIO &v1, const ArtNetIO &v2)
{
    return v1.address.ip().toString() < v2.address.ip().toString();
}

ArtNetPlugin::~ArtNetPlugin()
{
}

void ArtNetPlugin::init()
{
    QSettings settings;
    QVariant value = settings.value(SETTINGS_IFACE_WAIT_TIME);
    if (value.isValid() == true)
        m_ifaceWaitTime = value.toInt();
    else
        m_ifaceWaitTime = 0;

    foreach (QNetworkInterface iface, QNetworkInterface::allInterfaces())
    {
        foreach (QNetworkAddressEntry entry, iface.addressEntries())
        {
            QHostAddress addr = entry.ip();
            if (addr.protocol() != QAbstractSocket::IPv6Protocol)
            {
                ArtNetIO tmpIO;
                tmpIO.iface = iface;
                tmpIO.address = entry;
                tmpIO.controller = NULL;

                bool alreadyInList = false;
                for (int j = 0; j < m_IOmapping.count(); j++)
                {
                    if (m_IOmapping.at(j).address == tmpIO.address)
                    {
                        alreadyInList = true;
                        break;
                    }
                }
                if (alreadyInList == false)
                {
                    m_IOmapping.append(tmpIO);
                }
            }
        }
    }
    std::sort(m_IOmapping.begin(), m_IOmapping.end(), addressCompare);
}

QString ArtNetPlugin::name()
{
    return QString("ArtNet");
}

bool ArtNetPlugin::requestLine(quint32 line)
{
    int retryCount = 0;

    while (line >= (quint32)m_IOmapping.length())
    {
        qDebug() << "[ArtNet] cannot open line" << line << "(available:" << m_IOmapping.length() << ")";
        if (m_ifaceWaitTime)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            init();
        }
        if (retryCount++ >= m_ifaceWaitTime)
            return false;
    }

    return true;
}

/*********************************************************************
 * Outputs
 *********************************************************************/
QStringList ArtNetPlugin::outputs()
{
    QStringList list;

    init();

    foreach (ArtNetIO line, m_IOmapping)
        list << line.address.ip().toString();

    return list;
}

bool ArtNetPlugin::openOutput(quint32 output, quint32 universe)
{
    if (requestLine(output) == false)
        return false;

    qDebug() << "[ArtNet] Open output on address :" << m_IOmapping.at(output).address.ip().toString();

    // if the controller doesn't exist, create it
    if (m_IOmapping[output].controller == NULL)
    {
        ArtNetController *controller = new ArtNetController(m_IOmapping.at(output).iface,
                                                            m_IOmapping.at(output).address,
                                                            getUdpSocket(),
                                                            output, this);
        connect(controller, SIGNAL(valueChanged(quint32,quint32,quint32,uchar)),
                this, SIGNAL(valueChanged(quint32,quint32,quint32,uchar)));
        connect(controller, SIGNAL(rdmValueChanged(quint32, quint32, QVariantMap)),
                this , SIGNAL(rdmValueChanged(quint32, quint32, QVariantMap)));
        m_IOmapping[output].controller = controller;
    }

    m_IOmapping[output].controller->addUniverse(universe, ArtNetController::Output);

    return true;
}

void ArtNetPlugin::closeOutput(quint32 output, quint32 universe)
{
    if (output >= (quint32)m_IOmapping.length())
        return;

    ArtNetController *controller = m_IOmapping.at(output).controller;
    if (controller != NULL)
    {
        controller->removeUniverse(universe, ArtNetController::Output);
        if (controller->universesList().count() == 0)
        {
            delete m_IOmapping[output].controller;
            m_IOmapping[output].controller = NULL;
        }
    }
}

void ArtNetPlugin::writeUniverse(quint32 universe, quint32 output, const QByteArray &data, bool dataChanged)
{
    if (output >= (quint32)m_IOmapping.count())
        return;

    ArtNetController *controller = m_IOmapping.at(output).controller;
    if (controller != NULL)
        controller->sendDmx(universe, data, dataChanged);
}


/*********************************************************************
 * Configuration
 *********************************************************************/
void ArtNetPlugin::configure()
{
    // ConfigureArtNet conf(this);
    // conf.exec();
}


void ArtNetPlugin::setParameter(quint32 universe, quint32 line,
                                QString name, QVariant value)
{
    if (line >= (quint32)m_IOmapping.length())
        return;

    ArtNetController *controller = m_IOmapping.at(line).controller;
    if (controller == NULL)
        return;

    // If the Controller parameter is restored to its default value,
    // unset the corresponding plugin parameter
    bool unset;

    if (name == ARTNET_OUTPUTIP)
        unset = controller->setOutputIPAddress(universe, value.toString());
    else if (name == ARTNET_OUTPUTUNI)
        unset = controller->setOutputUniverse(universe, value.toUInt());
    else if (name == ARTNET_TRANSMITMODE)
        unset = controller->setTransmissionMode(universe, ArtNetController::stringToTransmissionMode(value.toString()));
    else
    {
        qWarning() << Q_FUNC_INFO << name << "is not a valid ArtNet output parameter";
        return;
    }
}

QList<ArtNetIO> ArtNetPlugin::getIOMapping()
{
    return m_IOmapping;
}

/********************************************************************
 * RDM
 ********************************************************************/

bool ArtNetPlugin::sendRDMCommand(quint32 universe, quint32 line, uchar command, QVariantList params)
{
    qDebug() << "Sending RDM command on universe" << universe << "and line" << line;
    if (line >= (quint32)m_IOmapping.count())
        return false;

    ArtNetController *controller = m_IOmapping.at(line).controller;
    if (controller != NULL)
        return controller->sendRDMCommand(universe, command, params);

    return false;
}

/*********************************************************************
 * ArtNet socket
 *********************************************************************/

QSharedPointer<QUdpSocket> ArtNetPlugin::getUdpSocket()
{
    // Is the socket already present ?
    QSharedPointer<QUdpSocket> udpSocket(m_udpSocket);
    if (udpSocket)
        return udpSocket;

    // Create a new socket
    udpSocket = QSharedPointer<QUdpSocket>(new QUdpSocket());
    m_udpSocket = udpSocket.toWeakRef();

    if (udpSocket->bind(ARTNET_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        connect(udpSocket.data(), SIGNAL(readyRead()),
                this, SLOT(slotReadyRead()));
    }
    else
    {
        qWarning() << "ArtNet: could not bind socket to address" << QString("0:%2").arg(ARTNET_PORT);
    }
    return udpSocket;
}

void ArtNetPlugin::slotReadyRead()
{
    QUdpSocket* udpSocket = qobject_cast<QUdpSocket*>(sender());
    Q_ASSERT(udpSocket != NULL);

    QByteArray datagram;
    QHostAddress senderAddress;
    while (udpSocket->hasPendingDatagrams())
    {
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress);
        handlePacket(datagram, senderAddress);
    }
}

void ArtNetPlugin::handlePacket(QByteArray const& datagram, QHostAddress const& senderAddress)
{
    // A first filter: look for a controller on the same subnet as the sender.
    // This allows having the same ArtNet Universe on 2 different network interfaces.
    foreach (ArtNetIO io, m_IOmapping)
    {
        if (senderAddress.isInSubnet(io.address.ip(), io.address.prefixLength()))
        {
            if (io.controller != NULL)
                io.controller->handlePacket(datagram, senderAddress);
            return;
        }
    }
    // Packet comming from another subnet. This is an unusual case.
    // We stop at the first controller that handles this packet.
    foreach (ArtNetIO io, m_IOmapping)
    {
        if (io.controller != NULL)
        {
            if (io.controller->handlePacket(datagram, senderAddress))
                break;
        }
    }
}
