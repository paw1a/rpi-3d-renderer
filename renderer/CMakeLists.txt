cmake_minimum_required(VERSION 3.21)
project(renderer)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} include)

add_executable(renderer
        main.cpp
        loader.cpp
        debug.cpp
        )

target_link_libraries(renderer PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
