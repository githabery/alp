add_executable(httpserver_test
    httpserver_test.cpp httpserver_test.h
)
target_include_directories(httpserver_test PUBLIC
    ../api
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_link_libraries(httpserver_test PUBLIC
    Qt6::Core
    Qt6::Network
    Qt::Test
    httpserver
)
