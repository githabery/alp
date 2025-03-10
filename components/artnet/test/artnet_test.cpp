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
#include "IDMX512Config.hpp"
#include "IDMX512Delivery.hpp"
#undef private

/****************************************************************************
 * ArtNet tests
 ****************************************************************************/

void ArtNet_Test::setupArtNetDmx()
{
    IDMX512Config& dmxConfig = IDMX512Config::instance();
    IDMX512Delivery& dmxDelivery = IDMX512Delivery::instance();

    quint32 universe = 4;
    quint32 channel = 6;

    std::vector<std::string> outs = dmxConfig.outputs();

    for(auto&& out : outs) {
        qDebug() << out;
        dmxConfig.openOutput(out, universe);
    }

    QByteArray data{512, 0};

    for (int var = 0; var < 2; ++var) {
        for(auto val = 0; val < 256; ++val) {
            data[channel] = val;
            for(quint32 i = 0; i < outs.size(); ++i) {
                dmxDelivery.writeUniverse(universe, data);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }

        for(auto val = 254; val > 0; --val) {
            data[channel] = val;
            for(quint32 i = 0; i < outs.size(); ++i) {
                dmxDelivery.writeUniverse(universe, data);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

}

QTEST_MAIN(ArtNet_Test)
