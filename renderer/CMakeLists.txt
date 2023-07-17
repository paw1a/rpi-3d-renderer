cmake_minimum_required(VERSION 3.21)
project(renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} math)

add_executable(renderer
        main.cpp
        loader.cpp
        debug.cpp
        pipeline.cpp
        render.cpp
        math/mat4.cpp
        math/quat.cpp
        math/transform.cpp
        math/vec3.cpp
        )

target_link_libraries(renderer PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
