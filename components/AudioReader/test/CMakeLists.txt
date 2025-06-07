add_executable(asioreader_test
    asioreader_test.cpp asioreader_test.h
)
target_include_directories(asioreader_test PUBLIC
    ../api
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_link_libraries(asioreader_test PUBLIC
    Qt6::Core
    Qt6::Network
    Qt::Test
    audioreader
)
