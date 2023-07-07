cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rpi_3d_renderer_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_executable(rpi_3d_renderer
        src/main.cpp
        )

pico_add_extra_outputs(rpi_3d_renderer)
target_link_libraries(rpi_3d_renderer pico_stdlib)
