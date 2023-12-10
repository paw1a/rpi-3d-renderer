#include <iostream>
#include <vector>

#include "gfx.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "st7789.h"
#include "pico/stdlib.h"

#include "debug.h"
#include "loader.h"
#include "pipeline.h"
#include "render.h"
#include "dataset.h"

#define COMMAND_MAX_SIZE 255
#define DISPLAY_COUNT 2

struct display {
    uint16_t width;
    uint16_t height;

    int16_t xstart;
    int16_t ystart;

    uint8_t rotation;

    spi_inst_t *spi;
    uint16_t pinDC;
    int16_t pinRST;
    uint16_t pinSCK;
    uint16_t pinTX;

    uint dma_tx;
    dma_channel_config dma_cfg;
};

std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
    size_t pos_start = 0;
    size_t pos_end;
    size_t delim_len = delimiter.length();

    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

static void set_pixel(display_t *display, point2 point, uint16_t color) {
    GFX_drawPixel(display, point.x + display->width / 2, point.y + display->height / 2,
                  color);
}

static struct state {
    struct dataset dataset{};
    struct scene scene;
    m3::mat4 rotate[2];
    m3::mat4 scale;
    char command[COMMAND_MAX_SIZE]{};
    size_t commandSize{};
    array<polygon> polygons[2];
} state;

static display_t displays[2];

static void print_usage() {
    std::cout << "\nRaspberry Pi Pico 3D" << std::endl;
    std::cout << "Usage:\n" << std::endl;
    std::cout << "help\tPrint help information" << std::endl;
    std::cout << "models\tPrint list of available 3D models" << std::endl;
    std::cout << "load\t<model name> -- Loads model by its name" << std::endl;
    std::cout << "camera set cp <x, y, z> ct <x, y, z> cu <x, y, z>"
                 " -- Set camera position with position, target, up vectors"
              << std::endl;
    std::cout << "camera rotate <rx, ry, rz>"
                 " -- Rotate camera, where rx, ry, rz - angles in degrees"
              << std::endl;
    std::cout << "camera scale <k>"
                 " -- Scale camera, where k - scale factor"
              << std::endl;
    std::cout << "camera reset -- Reset camera position to default\n" << std::endl;
}

static void execute_command() {
    std::string command(state.command);
    std::vector<std::string> tokens = split(command, " ");
    if (tokens.empty()) {
        return;
    }

    std::string operation = tokens[0];
    if (operation == "models") {
        std::cout << "list of available 3d models" << std::endl;
        for (size_t i = 0; i < DATASETS_SIZE; i++) {
            std::cout << datasets[i].name << std::endl;
        }
    } else if (operation == "load") {
        if (tokens.size() != 2) {
            std::cout << "usage: load <model name>" << std::endl;
            return;
        }

        std::string model = tokens[1];
        for (size_t i = 0; i < DATASETS_SIZE; i++) {
            if (datasets[i].name == model) {
                state.dataset = datasets[i];
                state.scene.objects.clear();
                state.scene.materials.clear();
                state.scene.lights.clear();
                if (!load_scene(state.dataset, state.scene)) {
                    std::cout << "failed to load scene file " << state.dataset.name << std::endl;
                    return;
                }

                size_t polygons_size = 0;
                for (auto &object : state.scene.objects)
                    polygons_size += object.faces.size();

                for (size_t i = 0; i < DISPLAY_COUNT; i++) {
                    delete[] state.polygons[i].data;
                    state.polygons[i] = {new polygon[polygons_size], polygons_size};
                }
                std::cout << "polygons count = " << polygons_size << std::endl;
                std::cout << std::endl;
                return;
            }
        }
        std::cout << "invalid model name, check models list" << std::endl;
    } else if (operation == "camera") {
        if (tokens.size() < 2) {
            std::cout << "invalid command arguments" << std::endl;
            return;
        }

        std::string key = tokens[1];
        if (key == "set") {
            if (tokens.size() != 11) {
                std::cout << "invalid camera set arguments" << std::endl;
                return;
            }

            m3::vec3 position = {
                std::stof(tokens[2]),
                std::stof(tokens[3]),
                std::stof(tokens[4])
            };
            m3::vec3 target = {
                std::stof(tokens[5]),
                std::stof(tokens[6]),
                std::stof(tokens[7])
            };
            m3::vec3 up = {
                std::stof(tokens[8]),
                std::stof(tokens[9]),
                std::stof(tokens[10])
            };
            state.scene.camera = {position, target, up};
            state.rotate[0] = m3::rotate_x(0) * m3::rotate_y(0) * m3::rotate_z(0);
            state.rotate[1] = m3::rotate_x(0) * m3::rotate_y(90 * 3.14f / 180) * m3::rotate_z(0);
            state.scale = m3::scale({2000, 2000, 2000});
        } else if (key == "rotate") {
            if (tokens.size() != 5) {
                std::cout << "invalid camera rotate arguments" << std::endl;
                return;
            }

            float radian = 3.14f / 180;
            m3::mat4 rotate_matrix = m3::rotate_z(std::stof(tokens[4]) * radian) *
                                     m3::rotate_y(std::stof(tokens[3]) * radian) *
                                     m3::rotate_x(std::stof(tokens[2]) * radian);
            for (auto &rotate : state.rotate) {
                rotate = rotate * rotate_matrix;
            }
        } else if (key == "scale") {
            if (tokens.size() != 3) {
                std::cout << "invalid camera scale arguments" << std::endl;
                return;
            }

            float value = std::stof(tokens[2]);
            m3::mat4 scale = m3::scale({value, value, value});
            state.scale = state.scale * scale;
        } else if (key == "reset") {
            state.rotate[0] = m3::rotate_x(0) * m3::rotate_y(0) * m3::rotate_z(0);
            state.rotate[1] = m3::rotate_x(0) * m3::rotate_y(90 * 3.14f / 180) * m3::rotate_z(0);
            state.scale = m3::scale({2000, 2000, 2000});
        } else {
            std::cout << "invalid camera option" << std::endl;
            return;
        }
    } else if (command == "help") {
        print_usage();
    } else {
        std::cout << "invalid command" << std::endl;
    }
    std::cout << std::endl;
}

static void on_uart_rx() {
    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);
        if (ch == 255) {
            return;
        }

        if (ch == '\r' || ch == '\n') {
            state.command[state.commandSize] = '\0';
            state.commandSize = 0;
            uart_putc(uart0, '\r');
            uart_putc(uart0, '\n');
            execute_command();
        } else if (state.commandSize >= COMMAND_MAX_SIZE - 1) {
            state.commandSize = 0;
            uart_putc(uart0, '\r');
            uart_putc(uart0, '\n');
        } else {
            state.command[state.commandSize++] = ch;
            if (uart_is_writable(uart0)) {
                uart_putc(uart0, ch);
            }
        }
    }
}

__attribute__((noreturn)) void idle() {
    for (;;)
        ;
}

int main() {
    stdio_init_all();

    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);

    std::cout << "Firmware started up!" << std::endl;
    print_usage();

    displays[0] = {
        .spi = spi0,
        .pinDC = 8,
        .pinRST = 9,
        .pinSCK = 6,
        .pinTX = 7
    };
    LCD_initDisplay(&displays[0], 240, 240);
    LCD_setRotation(&displays[0], 2);

    displays[1] = {
        .spi = spi1,
        .pinDC = 12,
        .pinRST = 13,
        .pinSCK = 10,
        .pinTX = 11
    };
    LCD_initDisplay(&displays[1], 240, 240);
    LCD_setRotation(&displays[1], 2);

    std::cout << "displayes inited" << std::endl;

    GFX_createFramebuf(&displays[0]);

    std::cout << "buffer inited" << std::endl;

    state.dataset = datasets[0];
    if (!load_scene(state.dataset, state.scene)) {
        std::cout << "failed to load scene file " << state.dataset.name << std::endl;
        idle();
    }

    size_t polygons_size = 0;
    for (auto &object : state.scene.objects)
        polygons_size += object.faces.size();

    for (size_t i = 0; i < DISPLAY_COUNT; i++) {
        state.polygons[i] = {new polygon[polygons_size], polygons_size};
    }
    std::cout << "polygons count = " << polygons_size << std::endl;

    state.rotate[0] = m3::rotate_x(0) * m3::rotate_y(0) * m3::rotate_z(0);
    state.rotate[1] = m3::rotate_x(0) * m3::rotate_y(90 * 3.14f / 180) * m3::rotate_z(0);
    state.scale = m3::scale({2000, 2000, 2000});

    for (;;) {
        for (size_t i = 0; i < 2; i++) {
            GFX_clearScreen(&displays[i]);

            m3::mat4 view = m3::look_at(
                m3::transform_vector(state.rotate[i],
                                     state.scene.camera.position),
                state.scene.camera.target,
                m3::transform_vector(state.rotate[i],
                                     state.scene.camera.up));
            m3::mat4 perspective = m3::perspective(80, 1, 1.1f, 10.0f);
            m3::mat4 transform = state.scale * perspective * view;

            for (auto &object : state.scene.objects) {
                for (auto &vertex : object.vertices) {
                    vertex = m3::transform_vector(transform, vertex);
                }
            }

            if (!scene_to_polygons(state.scene, state.polygons[i])) {
                std::cout << "failed to preprocess objects" << std::endl;
                idle();
            }

            transform = m3::inverse(transform);
            for (auto &object : state.scene.objects) {
                for (auto &vertex : object.vertices) {
                    vertex = m3::transform_vector(transform, vertex);
                }
            }
        }

        window window = {{-displays[0].width / 2, -displays[0].height / 2},
                          {displays[0].width / 2, displays[0].height / 2}};
        int16_t window_width = window.end.x - window.begin.x;
        int16_t window_height = window.end.y - window.begin.y;

        int16_t x_split1 = window.begin.x + (window_width / 3);
        int16_t x_split2 = window.begin.x + (window_width / 3) * 2;
        int16_t y_split = window.begin.y + (window_height / 2);

        struct window windows[DISPLAY_COUNT][6] = {{
                {{window.begin.x, window.begin.y}, {x_split1, y_split}, state.polygons[0]},
                {{x_split1, window.begin.y}, {x_split2, y_split}, state.polygons[0]},
                {{x_split2, window.begin.y}, {window.end.x, y_split}, state.polygons[0]},
                {{window.begin.x, y_split}, {x_split1, window.end.y}, state.polygons[0]},
                {{x_split1, y_split}, {x_split2, window.end.y}, state.polygons[0]},
                {{x_split2, y_split}, {window.end.x, window.end.y}, state.polygons[0]}
            }, {
                {{x_split1, window.begin.y}, {x_split2, y_split}, state.polygons[1]},
                {{x_split2, window.begin.y}, {window.end.x, y_split}, state.polygons[1]},
                {{window.begin.x, y_split}, {x_split1, window.end.y}, state.polygons[1]},
                {{x_split1, y_split}, {x_split2, window.end.y}, state.polygons[1]},
                {{x_split2, y_split}, {window.end.x, window.end.y}, state.polygons[1]},
                {{window.begin.x, window.begin.y}, {x_split1, y_split}, state.polygons[1]}
            }
        };

        for (size_t i = 0; i < 6; i++) {
            for (size_t j = 0; j < 2; j++) {
                window = windows[j][i];
                warnock_render(&displays[j], window, BLACK, set_pixel);
                GFX_flush_block(&displays[j], window.begin.x + displays[j].width / 2,
                                window.begin.y + displays[j].height / 2, 80, 120);
            }
        }
    }
}
