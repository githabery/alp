set(module_name "audioreader")

add_library(${module_name}
    STATIC
    BASSAsioReader.hpp
    BASSAsioReader.cpp
)

add_executable(asio_test WIN32 BASSAsioReader.cpp)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name} PUBLIC
    bassasio

)

target_link_libraries(asio_test PUBLIC
    bassasio
    bass
)
