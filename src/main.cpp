#include <iostream>
#include <vector>

#include "gfx.h"
#include "gfxfont.h"
#include "font.h"
#include "hardware/spi.h"
#include "ili9341.h"
#include "pico/stdlib.h"
#include "rtc.h"

#include "debug.h"
#include "loader.h"
#include "pipeline.h"
#include "render.h"

#define SPI_PORT spi0

#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 6
#define PIN_MOSI 7
#define PIN_DC 8
#define PIN_RST 9

extern uint8_t __bss_end__;
extern uint8_t __StackLimit;

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

    GFX_clearScreen();
    GFX_setCursor(0, 24);
    GFX_setFont(&free_mono_24pt);
    GFX_printf("LOADING...");
    GFX_flush();

//    char *ptr, *prev;
//    prev = new char[1024];
//    uint32_t num = 0;
//    while ((ptr = new char[1024]) != nullptr) {
//        num++;
//        if (ptr != prev + 1024)
//            std::cout << num << " " << std::hex << (uint32_t)ptr << std::endl;
//        prev = ptr;
//    }

    std::cout << "Start to load scene" << std::endl;

    scene scene;
    dataset dataset = datasets[1];
    if (!load_scene(dataset, scene)) {
        std::cout << "failed to load scene" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < scene.lights.size(); i++) {
        std::cout << scene.lights[i] << std::endl;
    }

    for (size_t i = 0; i < scene.materials.size(); i++) {
        std::cout << scene.materials[i] << std::endl;
    }

    for (size_t i = 0; i < scene.objects.size(); i++) {
        std::cout << scene.objects[i] << std::endl;
    }

    size_t polygons_size = 0;
    for (size_t i = 0; i < scene.objects.size(); i++)
        polygons_size += scene.objects[i].faces.size();

    std::cout << "polygons count = " << polygons_size << std::endl;
    array<polygon> polygons(polygons_size);
    size_t polygon_index = 0;
    for (size_t i = 0; i < scene.objects.size(); i++) {
        object object = scene.objects[i];
        for (size_t j = 0; j < object.faces.size(); j++) {
            face face = object.faces[j];
            polygons[polygon_index++].vertices =
                array<m3::tvec2<int16_t>>(face.vertex_indices.size());
        }
    }

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

        for (size_t i = 0; i < scene.objects.size(); i++) {
            object object = scene.objects[i];
            for (size_t j = 0; j < object.vertices.size(); j++) {
                object.vertices[j] =
                    m3::transform_vector(transform, object.vertices[j]);
            }
        }

        if (!scene_to_polygons(scene, polygons)) {
            std::cout << "failed to preprocess objects" << std::endl;
            return -1;
        }

        transform = m3::inverse(transform);
        for (size_t i = 0; i < scene.objects.size(); i++) {
            object object = scene.objects[i];
            for (size_t j = 0; j < object.vertices.size(); j++) {
                object.vertices[j] =
                    m3::transform_vector(transform, object.vertices[j]);
            }
        }

        angle += 5;

        warnock_render({{-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2},
                        {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                        polygons},
                       BLACK, set_pixel);

        GFX_flush();
    }
}
