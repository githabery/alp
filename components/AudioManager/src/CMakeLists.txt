set(module_name "audiomanager")

add_library(${module_name}
    STATIC
    AudioManager.cpp
)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name}
    PRIVATE
        Qt6::Core
        Qt6::Network
        audioprocessor
        audioreader
        persistency
)
