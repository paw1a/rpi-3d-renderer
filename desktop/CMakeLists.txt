cmake_minimum_required(VERSION 3.21)
project(desktop)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(RENDERER_SOURCES_PATH ..)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS}
        ${RENDERER_SOURCES_PATH}/src/math
        ${RENDERER_SOURCES_PATH}/src/render
        ${RENDERER_SOURCES_PATH}/src/scene)

add_executable(desktop
        main.cpp
        loader.cpp
        ${RENDERER_SOURCES_PATH}/src/render/debug.cpp
        ${RENDERER_SOURCES_PATH}/src/render/pipeline.cpp
        ${RENDERER_SOURCES_PATH}/src/render/render.cpp
        ${RENDERER_SOURCES_PATH}/src/math/mat4.cpp
        ${RENDERER_SOURCES_PATH}/src/math/quat.cpp
        ${RENDERER_SOURCES_PATH}/src/math/transform.cpp
        ${RENDERER_SOURCES_PATH}/src/math/vec3.cpp
        ${RENDERER_SOURCES_PATH}/src/scene/scene.cpp
        )

target_link_libraries(desktop PRIVATE SDL2::SDL2)
