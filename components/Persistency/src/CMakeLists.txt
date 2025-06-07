set(module_name "persistency")

add_library(${module_name}
    STATIC
    Persistency.cpp
)

target_include_directories(${module_name} PUBLIC ../api)

target_link_libraries(${module_name}
    PRIVATE
        Qt6::Core
        Qt6::Network
        audiomanager
)
