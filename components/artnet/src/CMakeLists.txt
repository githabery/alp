set(module_name "artnet")

add_library(${module_name}
    STATIC
    artnetcontroller.cpp artnetcontroller.h
    artnetpacketizer.cpp artnetpacketizer.h
    artnetplugin.cpp artnetplugin.h
    rdmprotocol.cpp rdmprotocol.h
)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name} PRIVATE
    Qt6::Core
    Qt6::Network
)
