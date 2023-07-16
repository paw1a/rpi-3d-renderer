#pragma once

#include "mat4.h"
#include "quat.h"
#include "vec3.h"

namespace m3 {
struct transform {
    vec3 position;
    quat rotation;
    vec3 scale;
    transform()
        : position(vec3(0.0)), rotation(quat(0, 0, 0, 1)), scale(vec3(1.0)) {
    }
    transform(const vec3 &p, const quat &r)
        : position(p), rotation(r), scale(vec3(1.0)) {
    }
    transform(const vec3 &p, const quat &r, const vec3 &s)
        : position(p), rotation(r), scale(s) {
    }
};

transform combine(const transform &parent, const transform &curr);
transform operator*(const transform &parent, const transform &curr);
transform inverse(const transform &t);
transform mix(const transform &a, const transform &b, float t);
bool operator==(const transform &a, const transform &b);
bool operator!=(const transform &a, const transform &b);
mat4 transform_to_mat4(const transform &t);
transform mat4_to_transform(const mat4 &m);
vec3 transform_point(const transform &a, const vec3 &b);
vec3 transform_vector(const transform &a, const vec3 &b);
} // namespace m3
