#include "BASSAsioReader.hpp"


// Example usage
int main() {
    BASSAsioReader handler;

    // Get list of devices
    auto devices = handler.getAsioDevices();
    std::cout << "ASIO Devices:" << std::endl;
    for (const auto& device : devices) {
        std::cout << " - " << device << std::endl;
    }

    // Set device
    if (!handler.setAsioDevice(0)) {
        return -1;
    }

    int channels = handler.getInputChannelCount();

    std::cout << "channels: " << channels << std::endl;

    // Subscribe to audio data
    handler.subscribeToAudioData(0, [](const float* data, int size) {
        std::cout << "Received audio data, size: " << size << std::endl;
    });

    // Subscribe to FFT data
    handler.subscribeToFFTData(0, [](const float* fft, int size) {
        std::cout << "Received FFT data, size: " << size << std::endl;
    });

    // Unsubscribe from audio and FFT data after a user-defined point
    std::cout << "Press Enter to unsubscribe..." << std::endl;
    std::cin.get();
    handler.unsubscribeFromAudioData(0);
    handler.unsubscribeFromFFTData(0);

    return 0;
}
