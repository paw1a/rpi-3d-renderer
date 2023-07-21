#pragma once

#include "color.h"
#include "math3d.h"
#include <vector>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

using point2 = m3::tvec2<int16_t>;

struct line2 {
    point2 begin;
    point2 end;
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
    array<polygon> polygons{};
};
