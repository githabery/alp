#include <bassasio.h>
#include <bass.h>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>

class BASSAsioReader {
public:
    BASSAsioReader()
    {}

    ~BASSAsioReader() {
        BASS_ASIO_Free();
    }

    // Method 1: Get list of ASIO devices
    std::vector<std::string> getAsioDevices() {
        std::vector<std::string> devices;
        BASS_ASIO_DEVICEINFO info;
        for (int i = 0; BASS_ASIO_GetDeviceInfo(i, &info); ++i) {
            devices.emplace_back(info.name);
        }
        return devices;
    }

    // Method 2: Get sample rate
    float getSampleRate() {
        return BASS_ASIO_GetRate();
    }

    int getInputChannelCount() const {
        BASS_ASIO_INFO info = {0};
        if (!BASS_ASIO_GetInfo(&info)) {
            return 0;
        }
        return info.inputs;
    }

    // Method 5: Set the ASIO device
    bool setAsioDevice(int deviceIndex) {



        if (!BASS_ASIO_Init(deviceIndex, BASS_ASIO_THREAD)) {
            std::cerr << "Failed to init ASIO device: " << BASS_ASIO_ErrorGetCode() << std::endl;
            return false;
        }

        if (!BASS_ASIO_SetDevice(deviceIndex)) {
            std::cerr << "Failed to set ASIO device: " << BASS_ASIO_ErrorGetCode() << std::endl;
            return false;
        }
        return true;
    }

    // Method 6: Subscribe to audio input data
    bool subscribeToAudioData(int channel, std::function<void(const float*, int)> callback) {
        if (audioCallbacks.find(channel) != audioCallbacks.end()) {
            std::cerr << "Audio data subscription already exists for channel " << channel << std::endl;
            return false;
        }

        // if (!BASS_ASIO_ChannelEnable(false, channel, [](BOOL input, DWORD channel, void* buffer, DWORD length, void* user) {
        //         auto cb = static_cast<std::function<void(const float*, int)>*>(user);
        //         (*cb)(static_cast<float*>(buffer), length / sizeof(float));
        //     }, &audioCallbacks[channel])) {
        //     std::cerr << "Failed to enable ASIO channel: " << BASS_ASIO_ErrorGetCode() << std::endl;
        //     return false;
        // }

        audioCallbacks[channel] = callback;
        return true;
    }

    //(BOOL input, DWORD channel, void *buffer, DWORD length, void *user);
    // Method 7: Subscribe to FFT data
    bool subscribeToFFTData(int channel, std::function<void(const float*, int)> callback, int fftSize = 1024) {
        if (fftCallbacks.find(channel) != fftCallbacks.end()) {
            std::cerr << "FFT data subscription already exists for channel " << channel << std::endl;
            return false;
        }

        // if (!BASS_ASIO_ChannelEnable(false, channel, [](BOOL input, DWORD channel, void* buffer, DWORD length, void* user) {
        //         auto cb = static_cast<std::function<void(const float*, int)>*>(user);
        //         static float fft[1024];
        //         BASS_ChannelGetData(0, fft, BASS_DATA_FFT1024);
        //         (*cb)(fft, fftSize);
        //     }, &fftCallbacks[channel])) {
        //     std::cerr << "Failed to enable ASIO channel for FFT: " << BASS_ASIO_ErrorGetCode() << std::endl;
        //     return false;
        // }

        fftCallbacks[channel] = callback;
        return true;
    }

    // Method to unsubscribe from audio data
    bool unsubscribeFromAudioData(int channel) {
        if (audioCallbacks.find(channel) == audioCallbacks.end()) {
            std::cerr << "No active audio data subscription for channel " << channel << std::endl;
            return false;
        }

        // if (!BASS_ASIO_ChannelDisable(channel)) {
        //     std::cerr << "Failed to disable ASIO channel: " << BASS_ASIO_ErrorGetCode() << std::endl;
        //     return false;
        // }

        audioCallbacks.erase(channel);
        return true;
    }

    // Method to unsubscribe from FFT data
    bool unsubscribeFromFFTData(int channel) {
        if (fftCallbacks.find(channel) == fftCallbacks.end()) {
            std::cerr << "No active FFT data subscription for channel " << channel << std::endl;
            return false;
        }

        // if (!BASS_ASIO_ChannelDisable(channel)) {
        //     std::cerr << "Failed to disable ASIO channel: " << BASS_ASIO_ErrorGetCode() << std::endl;
        //     return false;
        // }

        fftCallbacks.erase(channel);
        return true;
    }

private:
    std::unordered_map<int, std::function<void(const float*, int)>> audioCallbacks;
    std::unordered_map<int, std::function<void(const float*, int)>> fftCallbacks;
};
