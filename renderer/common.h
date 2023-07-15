#pragma once

#include "color.h"
#include <vector>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

struct point3 {
    float x;
    float y;
    float z;
};

using vec3 = point3;
using vertex = point3;

struct point2 {
    uint16_t x;
    uint16_t y;
};

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
    std::vector<point3> vertices;
    std::vector<vec3> normals;
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
