set(module_name "audioreader")

add_library(${module_name}
    STATIC
    BASSAsioReader.hpp
    BASSAsioReader.cpp
)

target_include_directories(${module_name} PUBLIC ./)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name} PUBLIC
    Qt6::Core
    Qt6::Network
    bassasio
)
