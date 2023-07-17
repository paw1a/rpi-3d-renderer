#pragma once

#include "color.h"
#include "math3d.h"
#include <vector>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

using point2 = m3::tvec2<int16_t>;

struct line2 {
    point2 begin;
    point2 end;
};

struct face {
    std::vector<size_t> vertex_indices;
    size_t normal_index;
    material material;
};

struct object {
    std::vector<face> faces;
    std::vector<m3::vec3> vertices;
    std::vector<m3::vec3> normals;
};

struct polygon {
    std::vector<point2> vertices;
    uint16_t color;
    float a, b, c, d;
};

template <typename T>
struct array {
    T *data;
    size_t size;
};

struct window {
    // top left corner
    point2 begin;
    // bottom right corner
    point2 end;
    std::vector<polygon> polygons;
};
