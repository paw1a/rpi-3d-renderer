#include <SDL.h>
#include <fstream>
#include <map>
#include <vector>

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
    std::map<std::string, material> materials;
    std::ifstream material_file("../cube.mtl", std::ios::in);
    if (!load_materials(material_file, materials)) {
        printf("failed to load materials\n");
        return -1;
    }

    std::vector<m3::vec3> lights = {
        {1, 1, 1},
        {-1, -1, -1},
        {-1, -1, -1},
    };

    std::map<std::string, object> objects;
    std::ifstream object_file("../cube.obj", std::ios::in);
    if (!load_objects(object_file, materials, objects)) {
        printf("failed to load objects\n");
        return -1;
    }

    for (auto const &[name, object] : objects) {
        std::cout << name << std::endl;
        std::cout << object << std::endl;
    }

    //    for (auto &polygon : polygons)
    //        std::cout << polygon << std::endl;

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

    m3::vec3 camera_pos = {0, 0, 20};
    m3::vec3 target = {0, 1, 0};
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
                    break;
                }
            }
        }

        angle += 0.5;
        m3::mat4 camara_rotate =
            m3::rotate_y(m3::deg2rad(angle)) * m3::rotate_x(m3::deg2rad(angle));
        m3::mat4 scale = m3::scale({2000, 2000, 2000});
        m3::mat4 view =
            m3::look_at(m3::transform_vector(camara_rotate, camera_pos),
                        m3::transform_vector(camara_rotate, target), {0, 1, 0});
        m3::mat4 perspective = m3::perspective(80, 1, 1.1f, 10.0f);
        m3::mat4 transform = scale * perspective * view;

        std::map<std::string, object> temp_objects(objects);
        for (auto &[name, object] : temp_objects) {
            for (auto &vertice : object.vertices) {
                vertice = m3::transform_vector(transform, vertice);
            }
        }

        std::vector<polygon> polygons;
        if (!preprocess_objects(temp_objects, lights, polygons)) {
            printf("failed to preprocess objects\n");
            return -1;
        }

        warnock_render({{-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2},
                        {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                        polygons},
                       BLACK, set_pixel);

        SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
