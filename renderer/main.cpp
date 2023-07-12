#include <SDL.h>
#include <fstream>
#include <map>
#include <vector>

#include "common.h"
#include "debug.h"
#include "loader.h"
#include "pipeline.h"
#include "render.h"

static uint32_t sdl_color_to_uint32(SDL_Color color) {
    uint32_t num = color.r;
    num |= (color.g << 8);
    num |= (color.b << 16);
    num |= (color.a << 24);

    return num;
}

int main() {
    std::map<std::string, object> objects;
    std::ifstream file("../monkey.obj", std::ios::in);
    if (!load_objects(file, objects)) {
        printf("failed to load objects\n");
        return -1;
    }

    adjust_data_to_display(objects);

    for (auto const &[name, object] : objects) {
        std::cout << name << std::endl;
        std::cout << object << std::endl;
    }

    std::vector<polygon> polygons;
    if (!preprocess_objects(objects, polygons)) {
        printf("failed to preprocess objects\n");
        return -1;
    }

    for (auto &polygon : polygons)
        std::cout << polygon << std::endl;

    auto *render_buffer = new uint32_t[SCREEN_HEIGHT * SCREEN_WIDTH];
    memset(render_buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
    init_render(render_buffer);

    window win = {{0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT}, polygons};
    warnock_render(win, 0x0);

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

    auto *pixels = new uint32_t[SCREEN_HEIGHT * SCREEN_WIDTH];

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

        memcpy(pixels, render_buffer,
               SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));

        SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
