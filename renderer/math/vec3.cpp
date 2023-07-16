#include "vec3.h"
#include "scalar.h"

namespace m3 {
vec3 operator+(const vec3 &l, const vec3 &r) {
    return {l.x + r.x, l.y + r.y, l.z + r.z};
}

vec3 operator-(const vec3 &l, const vec3 &r) {
    return {l.x - r.x, l.y - r.y, l.z - r.z};
}

vec3 operator-(vec3 v) {
    return {-v.x, -v.y, -v.z};
}

vec3 operator*(const vec3 &v, float f) {
    return {v.x * f, v.y * f, v.z * f};
}

vec3 operator*(float s, vec3 v) {
    return {v.x * s, v.y * s, v.z * s};
}

vec3 operator*(const vec3 &l, const vec3 &r) {
    return {l.x * r.x, l.y * r.y, l.z * r.z};
}

vec3 operator/(vec3 v, float s) {
    return {v.x / s, v.y / s, v.z / s};
}

vec3 operator/(float s, vec3 v) {
    return {s / v.x, s / v.y, s / v.z};
}
vec3 operator/(vec3 v, vec3 w) {
    return {v.x / w.x, v.y / w.y, v.z / w.z};
}
std::ostream &operator<<(std::ostream &output, const vec3 &v) {
    output << "vec3(" << v.x << "," << v.y << "," << v.z << ")";
    return output;
}

float dot(const vec3 &l, const vec3 &r) {
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

float len_sq(const vec3 &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const vec3 &v) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < VEC3_EPSILON) {
        return 0.0f;
    }
    return sqrtf(lenSq);
}

void normalize(vec3 &v) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < VEC3_EPSILON) {
        return;
    }
    float invLen = 1.0f / sqrtf(lenSq);

    v.x *= invLen;
    v.y *= invLen;
    v.z *= invLen;
}

vec3 normalized(const vec3 &v) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < VEC3_EPSILON) {
        return v;
    }
    float invLen = 1.0f / sqrtf(lenSq);

    return {v.x * invLen, v.y * invLen, v.z * invLen};
}

float angle(const vec3 &l, const vec3 &r) {
    float sqMagL = l.x * l.x + l.y * l.y + l.z * l.z;
    float sqMagR = r.x * r.x + r.y * r.y + r.z * r.z;

    if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) {
        return 0.0f;
    }

    float dot = l.x * r.x + l.y * r.y + l.z * r.z;
    float len = sqrtf(sqMagL) * sqrtf(sqMagR);
    return acosf(dot / len);
}

vec3 project(const vec3 &a, const vec3 &b) {
    float magBSq = len(b);
    if (magBSq < VEC3_EPSILON) {
        return {};
    }
    float scale = dot(a, b) / magBSq;
    return b * scale;
}

vec3 reject(const vec3 &a, const vec3 &b) {
    vec3 projection = project(a, b);
    return a - projection;
}

vec3 reflect(const vec3 &a, const vec3 &b) {
    float magBSq = len(b);
    if (magBSq < VEC3_EPSILON) {
        return {};
    }
    float scale = dot(a, b) / magBSq;
    vec3 proj2 = b * (scale * 2);
    return a - proj2;
}

vec3 cross(const vec3 &l, const vec3 &r) {
    return {l.y * r.z - l.z * r.y, l.z * r.x - l.x * r.z,
                l.x * r.y - l.y * r.x};
}

vec3 lerp(const vec3 &s, const vec3 &e, float t) {
    return {s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t,
                s.z + (e.z - s.z) * t};
}

vec3 slerp(const vec3 &s, const vec3 &e, float t) {
    if (t < 0.01f) {
        return lerp(s, e, t);
    }

    vec3 from = normalized(s);
    vec3 to = normalized(e);

    float theta = angle(from, to);
    float sin_theta = sinf(theta);

    float a = sinf((1.0f - t) * theta) / sin_theta;
    float b = sinf(t * theta) / sin_theta;

    return from * a + to * b;
}

vec3 nlerp(const vec3 &s, const vec3 &e, float t) {
    vec3 linear(s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t,
                s.z + (e.z - s.z) * t);
    return normalized(linear);
}

bool operator==(const vec3 &l, const vec3 &r) {
    vec3 diff(l - r);
    return len_sq(diff) < VEC3_EPSILON;
}

bool operator!=(const vec3 &l, const vec3 &r) {
    return !(l == r);
}

vec3 min(vec3 v, vec3 w) {
    return {min(v.x, w.x), min(v.y, w.y), min(v.z, w.z)};
}

vec3 max(vec3 v, vec3 w) {
    return {max(v.x, w.x), max(v.y, w.y), max(v.z, w.z)};
}

vec3 clamp(vec3 v, vec3 min, vec3 max) {
    return {clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y),
                clamp(v.z, min.z, max.z)};
}

void ortho_normalize(vec3 &normal, vec3 &tangent) {
    normalize(normal);
    tangent = tangent - project(tangent, normal);
    normalize(tangent);
}

vec3 rad2deg(vec3 rad) {
    return rad * 180 / PI;
}

vec3 deg2rad(vec3 deg) {
    return deg * PI / 180;
}
}
