add_executable(audiomanager_test
    audiomanager_test.cpp audiomanager_test.h
)
target_include_directories(audiomanager_test PUBLIC
    ../api
)

find_package(Qt6 REQUIRED COMPONENTS Test)

target_link_libraries(audiomanager_test PUBLIC
    Qt6::Core
    Qt6::Network
    Qt::Test
    audioprocessor
    audioreader
    audiomanager
    persistency
)
