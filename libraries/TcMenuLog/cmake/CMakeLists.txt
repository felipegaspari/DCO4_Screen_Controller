cmake_minimum_required(VERSION 3.13)

add_library(TcMenuLog 
        ../src/IoLogging.cpp
        ../src/TextUtilities.cpp
)

target_compile_definitions(TcMenuLog
        PUBLIC BUILD_FOR_PICO_CMAKE=1 BUILD_PICO_FORCE_UART=1 IO_LOGGING_DEBUG=1
)

target_include_directories(TcMenuLog PUBLIC
        ../src
)

target_link_libraries(TcMenuLog PUBLIC pico_stdlib pico_sync)
