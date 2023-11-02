#include <iostream>
#include <vector>

#include "gfx.h"
#include "hardware/spi.h"
#include "ili9341.h"
#include "pico/stdlib.h"
#include "rtc.h"

#include "debug.h"
#include "loader.h"
#include "pipeline.h"
#include "render.h"
#include "dataset.h"

#define SPI_PORT spi0

#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 6
#define PIN_MOSI 7
#define PIN_DC 8
#define PIN_RST 9

static void set_pixel(point2 point, uint16_t color) {
    GFX_drawPixel(point.x + SCREEN_WIDTH / 2, point.y + SCREEN_HEIGHT / 2,
                  color);
}

int main() {
    stdio_init_all();
    time_init();

    std::cout << "Hello world" << std::endl;

    LCD_setPins(PIN_DC, PIN_CS, PIN_RST, PIN_SCK, PIN_MOSI);
    LCD_setSPIperiph(SPI_PORT);
    LCD_initDisplay();
    LCD_setRotation(3);
    GFX_createFramebuf();

    scene scene;
    dataset dataset = datasets[2];
    if (!load_scene(dataset, scene)) {
        std::cout << "failed to load scene file " << dataset.name << std::endl;
        return -1;
    }

    for (auto const &object : scene.objects) {
        std::cout << object << std::endl;
    }

    size_t polygons_size = 0;
    for (auto &object : scene.objects)
        polygons_size += object.faces.size();

    array<polygon> polygons = {new polygon[polygons_size], polygons_size};
    std::cout << "polygons count = " << polygons_size << std::endl;

    float angle = 0;
    for (;;) {
        GFX_clearScreen();

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
            std::cout << "failed to preprocess objects" << std::endl;
            return -1;
        }

        transform = m3::inverse(transform);
        for (auto &object : scene.objects) {
            for (auto &vertice : object.vertices) {
                vertice = m3::transform_vector(transform, vertice);
            }
        }

        angle += 5;

        warnock_render({{-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2},
                        {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                        polygons},
                       BLACK, set_pixel);

        GFX_setCursor(0, 0);
        GFX_printf("Raspberry Pi Pico\nFPS: %d", 23);
        GFX_flush();
    }
}
