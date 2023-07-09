cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include(pico_libs_import.cmake)

project(rpi_3d_renderer_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

include_directories(include)

add_executable(rpi_3d_renderer
        src/main.cpp
        )

pico_add_extra_outputs(rpi_3d_renderer)
target_link_libraries(rpi_3d_renderer pico_stdlib pico_stdio ili934x hardware_gpio hardware_spi)
