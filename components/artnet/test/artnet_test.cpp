/*
  Q Light Controller Plus
  artnet_test.cpp

  Copyright (c) Jano Svitok

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

#include <QtTest/QTest>
#include <thread>

#define private public
#include "artnet_test.h"
#include "artnetplugin.h"
#undef private

/****************************************************************************
 * ArtNet tests
 ****************************************************************************/

void ArtNet_Test::setupArtNetDmx()
{
    ArtNetPlugin artPlug;
    quint32 universe = 4;
    quint32 channel = 6;

    QStringList outs = artPlug.outputs();

    for(auto&& out : outs) {
        qDebug() << out;
    }

    for(quint32 i = 0; i < outs.size(); ++i) {
        artPlug.openOutput(i, universe);
    }

    QByteArray data{512, 0};

    for (int var = 0; var < 2; ++var) {
        for(auto val = 0; val < 256; ++val) {
            data[channel] = val;
            for(quint32 i = 0; i < outs.size(); ++i) {
                artPlug.writeUniverse(universe, i, data, true);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }

        for(auto val = 254; val > 0; --val) {
            data[channel] = val;
            for(quint32 i = 0; i < outs.size(); ++i) {
                artPlug.writeUniverse(universe, i, data, true);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

}

QTEST_MAIN(ArtNet_Test)
