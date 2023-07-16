#pragma once

namespace m3 {
template <typename T>
struct tvec4 {
    union {
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        T v[4];
    };
    inline tvec4<T>() : x((T)0), y((T)0), z((T)0), w((T)0) {
    }
    inline tvec4<T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {
    }
    inline explicit tvec4<T>(T *fv) : x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) {
    }
};

typedef tvec4<float> vec4;
typedef tvec4<int> ivec4;

}
