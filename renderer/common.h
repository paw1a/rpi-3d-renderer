#pragma once

#include <vector>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

struct point3 {
    float x;
    float y;
    float z;
};

using vec3 = point3;
using color = point3;
using vertex = point3;

struct point2 {
    uint32_t x;
    uint32_t y;
};

struct face {
    std::vector<size_t> vertex_indices;
    size_t normal_index;
    color color;
};

struct object {
    std::vector<face> faces;
    std::vector<point3> vertices;
    std::vector<vec3> normals;
};

struct polygon {
    std::vector<point2> vertices;
    uint32_t color;
    float a, b, c, d;
};

struct window {
    point2 begin;
    point2 end;
    std::vector<polygon> polygons;
};
