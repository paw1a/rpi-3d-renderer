cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include(pico_libs_import.cmake)

project(renderer C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

pico_sdk_init()

set(COMMON_SOURCES
        src/render/debug.cpp
        src/render/pipeline.cpp
        src/render/render.cpp
        src/math/mat4.cpp
        src/math/quat.cpp
        src/math/transform.cpp
        src/math/vec3.cpp
        src/scene/scene.cpp
        )

set(RPI_PICO_SOURCES
        src/main.cpp
        src/loader.cpp
        src/hw_config.cpp
        )

add_executable(rpi-pico ${COMMON_SOURCES} ${RPI_PICO_SOURCES})

pico_add_extra_outputs(rpi-pico)

include_directories(src/math src/render src/scene)

add_subdirectory(lib/fatfs-spi build)

target_link_libraries(rpi-pico PRIVATE
        pico_stdlib
        pico_stdio
        ili934x
        hardware_gpio
        hardware_spi
        fatfs_spi
        )

#add_subdirectory(desktop)
#add_subdirectory(lib/ili9341)
