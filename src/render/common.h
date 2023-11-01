#pragma once

#include "color.h"
#include "math3d.h"
#include "array.h"
#include "allocator.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

using point2 = m3::tvec2<int16_t>;

struct line2 {
    point2 begin;
    point2 end;
};

template <typename T>
struct array {
    array() = default;
    explicit array(size_t len) {
        this->len = len;
        this->data = (T *)allocate(len * sizeof(T));
    }

    array(T *data, size_t len) {
        this->len = len;
        this->data = data;
    }

    T &operator[](size_t index) {
        return data[index];
    }

    const T &operator[](size_t index) const {
        return data[index];
    }

    [[nodiscard]] size_t size() const {
        return len;
    }

    T *getData() const {
        return data;
    }
  private:
    T *data;
    size_t len{};
};

struct polygon {
    array<point2> vertices;
    uint16_t color{};
    float a{}, b{}, c{}, d{};
};

struct window {
    // top left corner
    point2 begin;
    // bottom right corner
    point2 end;
    array<polygon> polygons;
};
