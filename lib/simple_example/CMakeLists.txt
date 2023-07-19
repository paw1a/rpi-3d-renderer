# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/home/carlk/pi/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(simple_example C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(../fatfs-spi build)

# Add executable. Default name is the project name, version 0.1
add_executable(simple_example
        simple_example.cpp
        hw_config.c
)

# Add the standard library and FatFS/SPI to the build
target_link_libraries(simple_example 
    pico_stdlib
    FatFs_SPI
)

pico_set_program_name(simple_example "simple_example")
pico_set_program_version(simple_example "0.1")

pico_add_extra_outputs(simple_example)
