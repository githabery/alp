set(module_name "audioprocessor")

add_library(${module_name}
    STATIC
    AudioProcessor.cpp
    kiss_fft.c kiss_fft.h
    kiss_fftr.c kiss_fftr.h
    kfc.c kfc.h
    kiss_fftnd.c kiss_fftnd.h
    kiss_fftndr.c kiss_fftndr.h
    kiss_fft_log.h
    _kiss_fft_guts.h
)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name}
    PRIVATE
        Qt6::Core
        Qt6::Network
        artnet
)
