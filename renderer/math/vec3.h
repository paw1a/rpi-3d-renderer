#pragma once
#include <iostream>

namespace m3 {
#define VEC3_EPSILON 0.000001f

struct vec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
    inline vec3() : x(0.0f), y(0.0f), z(0.0f) {
    }
    inline vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {
    }
    inline vec3(float s) : x(s), y(s), z(s) {
    }
    inline vec3(float *fv) : x(fv[0]), y(fv[1]), z(fv[2]) {
    }

    friend std::ostream &operator<<(std::ostream &output, const vec3 &v);

    inline void add_(const vec3 &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }

    inline static vec3 left() {
        return vec3{1.0f, 0.0f, 0.0f};
    };
    inline static vec3 up() {
        return vec3{0.0f, 1.0f, 0.0f};
    };
    inline static vec3 forward() {
        return vec3{0.0f, 0.0f, 1.0f};
    };
};

vec3 operator+(const vec3 &l, const vec3 &r);
vec3 operator-(const vec3 &l, const vec3 &r);
vec3 operator-(vec3 v);
vec3 operator*(const vec3 &v, float s);
vec3 operator*(float s, vec3 v);
vec3 operator*(const vec3 &l, const vec3 &r);
vec3 operator/(vec3 v, float s);
vec3 operator/(float s, vec3 v);
vec3 operator/(vec3 v, vec3 w);
float dot(const vec3 &l, const vec3 &r);
float len_sq(const vec3 &v);
float len(const vec3 &v);
void normalize(vec3 &v);
vec3 normalized(const vec3 &v);
float angle(const vec3 &l, const vec3 &r);
vec3 project(const vec3 &a, const vec3 &b);
vec3 reject(const vec3 &a, const vec3 &b);
vec3 reflect(const vec3 &a, const vec3 &b);
vec3 cross(const vec3 &l, const vec3 &r);
vec3 lerp(const vec3 &s, const vec3 &e, float t);
vec3 slerp(const vec3 &s, const vec3 &e, float t);
vec3 nlerp(const vec3 &s, const vec3 &e, float t);
bool operator==(const vec3 &l, const vec3 &r);
bool operator!=(const vec3 &l, const vec3 &r);
vec3 min(vec3 v, vec3 w);
vec3 max(vec3 v, vec3 w);
vec3 clamp(vec3 v, vec3 min, vec3 max);
void ortho_normalize(vec3 &normal, vec3 &tangent);

vec3 rad2deg(vec3 rad);
vec3 deg2rad(vec3 deg);
}
