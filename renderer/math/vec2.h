#pragma once

namespace m3 {
template <typename T>
struct tvec2 {
    union {
        struct {
            T x;
            T y;
        };
        T v[2];
    };
    inline tvec2() : x(T(0)), y(T(0)) {
    }
    inline tvec2(T _x, T _y) : x(_x), y(_y) {
    }
    inline tvec2(T *fv) : x(fv[0]), y(fv[1]) {
    }
};

typedef tvec2<float> vec2;
typedef tvec2<int> ivec2;

}