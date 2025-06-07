add_executable(audioprocessor_test
    audioprocessor_test.cpp audioprocessor_test.h
)
target_include_directories(audioprocessor_test PUBLIC
    ../api
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_link_libraries(audioprocessor_test PUBLIC
    Qt6::Core
    Qt6::Network
    Qt::Test
    audioprocessor
    audioreader
    audiomanager
)
