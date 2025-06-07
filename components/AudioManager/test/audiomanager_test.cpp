#include <QtTest/QTest>
#include <cmath>
#include <iostream>
#include <thread>

#define private public
#include "audiomanager_test.h"
#include "IAudioManager.hpp"
#include "IPersistency.hpp"
#include "ChannelInfo.hpp"
#undef private

/****************************************************************************
 * Asio tests
 ****************************************************************************/


void AudioManager_Test::manage()
{
    IAudioManager& manager = IAudioManager::create();
    manager.setAsioDevice(0);
std::cout << "Audio effect added, Audio processing started" << std::endl;
    //manager.loadSettings();
    //std::cout << "Settings loaded successfully" << std::endl;
    if(manager.addChannel(0)) {
        //std::cout << "Channel added" << std::endl;
    }
    //manager.addFunctionToChannel(0, AudioFunctionType::PeakDetector);
    //std::cout << "Processing audio..." << std::endl;
    if(manager.removeChannel(0)) {
        //std::cout << "Channel removed" << std::endl;
    }

    //manager.saveSettings();
    //std::cout << "Settings saved successfully" << std::endl;
    manager.removeAllChannels();

}

QTEST_MAIN(AudioManager_Test)
