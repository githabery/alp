cmake_minimum_required(VERSION 3.16)

project(AudioLightProcessor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Network)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(AudioLightProcessor
  main.cpp
  components/artnet/api/IDMX512Config.hpp
  components/artnet/api/IDMX512Delivery.hpp
)

add_subdirectory(components)

target_link_libraries(AudioLightProcessor Qt6::Core)

