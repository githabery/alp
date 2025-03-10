add_library(bassasio SHARED IMPORTED GLOBAL)

set_property(TARGET bassasio PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/include")
set_property(TARGET bassasio PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/lib/bassasio.dll")
set_property(TARGET bassasio PROPERTY IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/lib/bassasio.lib")


add_library(bass SHARED IMPORTED GLOBAL)

set_property(TARGET bass PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/include")
set_property(TARGET bass PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/lib/bass.dll")
set_property(TARGET bass PROPERTY IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/thirdparty/bassasio/lib/bass.lib")
