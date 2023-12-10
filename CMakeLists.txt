cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(renderer C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

pico_sdk_init()

add_subdirectory(lib/st7789)

include_directories(
        src/math
        src/render
        src/scene
        src
)

add_executable(rpi-pico
        src/render/debug.cpp
        src/render/pipeline.cpp
        src/render/render.cpp
        src/math/mat4.cpp
        src/math/quat.cpp
        src/math/transform.cpp
        src/math/vec3.cpp
        src/scene/scene.cpp
        src/main.cpp
        src/loader.cpp
        src/dataset.cpp
        )

pico_add_extra_outputs(rpi-pico)

target_link_libraries(rpi-pico PRIVATE
        pico_stdlib
        pico_stdio
        hardware_gpio
        hardware_spi
        st7789
        gfx
        )

add_subdirectory(desktop)
