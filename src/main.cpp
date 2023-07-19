#include <iostream>
#include <vector>
#include <sstream>

#include "pico/stdlib.h"
#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"
#include "loader.h"
#include "debug.h"
#include "pipeline.h"
#include "render.h"

static void set_pixel(point2 point, uint16_t color) {

}

int main() {
    stdio_init_all();
    time_init();

    std::cout << "Hello world" << std::endl;

    sd_card_t *sd = sd_get_by_num(0);
    FRESULT result = f_mount(&sd->fatfs, sd->pcName, 1);
    if (result != FR_OK) {
        std::cout << "f_mount error: " << FRESULT_str(result) << " " << result << std::endl;
        return -1;
    }

    scene scene;
    std::string filename = "cube.scene";
    char *data = read_file(filename.c_str());
    std::istringstream iss(data);
    if (!load_scene(iss, scene)) {
        std::cout << "failed to load scene file " << filename << std::endl;
        return -1;
    }
    delete [] data;

    for (auto const &object : scene.objects) {
        std::cout << object << std::endl;
    }

    size_t polygons_size = 0;
    for (auto &object : scene.objects)
        polygons_size += object.faces.size();

    array<polygon> polygons = {new polygon[polygons_size], polygons_size};
    std::cout << "polygons count = " << polygons_size << std::endl;

    m3::mat4 camara_rotate = m3::rotate_y(m3::deg2rad(30)) * m3::rotate_x(m3::deg2rad(30));
    m3::mat4 scale = m3::scale({2000, 2000, 2000});
    m3::mat4 view =
            m3::look_at(m3::transform_vector(camara_rotate, scene.camera.position),
                        scene.camera.target, m3::transform_vector(camara_rotate, scene.camera.up));
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

    warnock_render({{-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2},
                    {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                    polygons},
                    BLACK, set_pixel);

    delete [] polygons.data;

    f_unmount(sd->pcName);
    std::cout << "Goodbye world" << std::endl;

    for (;;)
        ;
}
