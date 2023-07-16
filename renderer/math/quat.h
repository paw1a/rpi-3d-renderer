#pragma once

#include "mat4.h"
#include "vec3.h"

namespace m3 {
#define QUAT_EPSILON 0.000001f

struct quat {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        struct {
            vec3 vector;
            float scalar;
        };
        float v[4];
    };

    inline quat() : x(0), y(0), z(0), w(1) {
    }
    inline quat(float _x, float _y, float _z, float _w)
        : x(_x), y(_y), z(_z), w(_w) {
    }
    friend std::ostream &operator<<(std::ostream &output, const quat &q);
};

quat angle_axis(const vec3 &axis, float angle, bool degree = false);
quat from_to(const vec3 &from, const vec3 &to);
vec3 get_axis(const quat &quat);
float get_angle(const quat &quat, bool degree = false);
quat operator+(const quat &a, const quat &b);
quat operator-(const quat &a, const quat &b);
quat operator*(const quat &a, float b);
quat operator-(const quat &q);
bool operator==(const quat &left, const quat &right);
bool operator!=(const quat &a, const quat &b);
bool same_orientation(const quat &left, const quat &right);
float dot(const quat &a, const quat &b);
float len_sq(const quat &q);
float len(const quat &q);
void normalize(quat &q);
quat normalized(const quat &q);
quat conjugate(const quat &q);
quat inverse(const quat &q);
quat operator*(const quat &q1, const quat &q2);
vec3 operator*(const quat &q, const vec3 &v);
quat mix(const quat &from, const quat &to, float t);
quat nlerp(const quat &from, const quat &to, float t);
quat operator^(const quat &q, float f);
quat slerp(const quat &start, const quat &end, float t);
quat look_rotation(const vec3 &direcion, const vec3 &up);
mat4 quat_to_mat4(const quat &q);
quat mat4_to_quat(const mat4 &m);

quat quat_abs(quat x);
quat quat_exp(vec3 v, float eps = 1e-8f);
vec3 to_euler(const quat &q, const std::string &order = "XYZ",
              bool degree = true);

// Rotates a rotation from towards to.
quat rotate_towards(quat from, quat to, float max_degrees_delta);
float angle(quat a, quat b);
}
