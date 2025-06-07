cmake_minimum_required(VERSION 3.28)

project(AudioLightProcessor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/mingw_64")

find_package(Qt6 NAMES Qt6 REQUIRED COMPONENTS Core Network HttpServer)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(AudioLightProcessor
  main.cpp
)

add_subdirectory(thirdparty)
add_subdirectory(components)

target_link_libraries(AudioLightProcessor
  Qt6::Core
  Qt6::HttpServer
  bassasio bass
  httpserver
  audiomanager
  persistency
)

