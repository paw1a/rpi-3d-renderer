#include <SDL.h>
#include <chrono>
#include <fstream>
#include <map>

#include "common.h"
#include "debug.h"
#include "loader.h"
#include "math3d.h"
#include "pipeline.h"
#include "render.h"

static uint32_t sdl_color_to_uint32(SDL_Color color) {
    uint32_t num = color.r;
    num |= (color.g << 8);
    num |= (color.b << 16);
    num |= (color.a << 24);

    return num;
}

static uint32_t *pixels;

static void set_pixel(point2 point, uint16_t rgb565_color) {
    color rgb_color = rgb565_to_rgb(rgb565_color);
    SDL_Color sdl_color = {rgb_color.r, rgb_color.g, rgb_color.b, 255};
    pixels[(point.y + SCREEN_HEIGHT / 2) * SCREEN_WIDTH +
           (point.x + SCREEN_WIDTH / 2)] = sdl_color_to_uint32(sdl_color);
}

int main() {
    scene scene;
    std::string scene_path = "models/cube.scene";
    std::ifstream ifs(scene_path, std::ios::in);
    if (!ifs.is_open()) {
        std::cout << "failed to open scene file " << scene_path << std::endl;
        return -1;
    }

    if (!load_scene(ifs, scene)) {
        std::cout << "failed to load scene " << scene_path << std::endl;
        return -1;
    }

    for (auto const &object : scene.objects) {
        std::cout << object << std::endl;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("failed to init sdl: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Test Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("failed to create window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        printf("failed to create renderer: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == nullptr) {
        printf("failed to create texture: %s\n", SDL_GetError());
        return -1;
    }

    pixels = new uint32_t[SCREEN_HEIGHT * SCREEN_WIDTH];

    size_t polygons_size = 0;
    for (auto &object : scene.objects)
        polygons_size += object.faces.size();

    array<polygon> polygons = {new polygon[polygons_size], polygons_size};
    std::cout << "polygons count = " << polygons_size << std::endl;

    float angle = 0;

    bool quit = false;
    while (!quit) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_UP:
                    angle += 0.5;
                    break;
                }
            }
        }

        auto begin = std::chrono::steady_clock::now();

        m3::mat4 camara_rotate =
            m3::rotate_y(m3::deg2rad(angle)) * m3::rotate_x(m3::deg2rad(angle));
        m3::mat4 scale = m3::scale({2000, 2000, 2000});
        m3::mat4 view = m3::look_at(
            m3::transform_vector(camara_rotate, scene.camera.position),
            scene.camera.target,
            m3::transform_vector(camara_rotate, scene.camera.up));
        m3::mat4 perspective = m3::perspective(80, 1, 1.1f, 10.0f);
        m3::mat4 transform = scale * perspective * view;

        for (auto &object : scene.objects) {
            for (auto &vertice : object.vertices) {
                vertice = m3::transform_vector(transform, vertice);
            }
        }

        if (!scene_to_polygons(scene, polygons)) {
            printf("failed to preprocess objects\n");
            return -1;
        }

        transform = m3::inverse(transform);
        for (auto &object : scene.objects) {
            for (auto &vertice : object.vertices) {
                vertice = m3::transform_vector(transform, vertice);
            }
        }

        auto end = std::chrono::steady_clock::now();
//        std::cout << "preprocess time = "
//                  << std::chrono::duration_cast<std::chrono::microseconds>(
//                         end - begin)
//                         .count()
//                  << std::endl;

        begin = std::chrono::steady_clock::now();
        warnock_render({{-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2},
                        {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                        polygons},
                       BLACK, set_pixel);

        end = std::chrono::steady_clock::now();
//        std::cout << "render time = "
//                  << std::chrono::duration_cast<std::chrono::microseconds>(
//                         end - begin)
//                         .count()
//                  << std::endl;

        SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    delete[] polygons.data;
    delete[] pixels;
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
