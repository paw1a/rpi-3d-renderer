#pragma once
#include <cmath>

namespace m3 {

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON 0.000001f
#endif

inline float rad2deg(float rad) {
    return rad * 180 / PI;
}

inline float deg2rad(float degree) {
    return degree / 180 * PI;
}

inline float clamp(float x, float min, float max) {
    return x > max ? max : x < min ? min : x;
}

inline float min(float x, float y) {
    return x < y ? x : y;
}

inline float max(float x, float y) {
    return x > y ? x : y;
}

inline float abs(float x) {
    return x > 0 ? x : -x;
}

inline float atan2(float y, float x) {
    return atan2f(y, x);
}

inline float acos(float x) {
    return std::acos(x);
}
} // namespace m3