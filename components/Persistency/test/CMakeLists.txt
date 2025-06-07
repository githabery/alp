add_executable(persistency_test
    persistency_test.cpp persistency_test.h
)
target_include_directories(persistency_test PUBLIC
    ../api
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_link_libraries(persistency_test PUBLIC
    Qt6::Core
    Qt6::Network
    Qt::Test
)
