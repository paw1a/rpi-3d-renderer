#include "quat.h"
#include "scalar.h"

namespace m3 {
quat angle_axis(const vec3 &axis, float angle, bool degree) {
    if (degree)
        angle = deg2rad(angle);
    vec3 norm = normalized(axis);
    float s = sinf(angle * 0.5f);

    return {norm.x * s, norm.y * s, norm.z * s, cosf(angle * 0.5f)};
}

quat from_to(const vec3 &from, const vec3 &to) {
    vec3 f = normalized(from);
    vec3 t = normalized(to);

    if (f == t) {
        return {};
    } else if (f == t * -1.0f) {
        vec3 ortho = vec3(1, 0, 0);
        if (fabsf(f.y) < fabsf(f.x)) {
            ortho = vec3(0, 1, 0);
        }
        if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
            ortho = vec3(0, 0, 1);
        }

        vec3 axis = normalized(cross(f, ortho));
        return {axis.x, axis.y, axis.z, 0};
    }

    vec3 half = normalized(f + t);
    vec3 axis = cross(f, half);

    return {axis.x, axis.y, axis.z, dot(f, half)};
}

vec3 get_axis(const quat &quat) {
    return normalized(vec3(quat.x, quat.y, quat.z));
}

float get_angle(const quat &quat, bool degree) {
    if (degree)
        return rad2deg(2.0f * acosf(quat.w));
    else
        return 2.0f * acosf(quat.w);
}

quat operator+(const quat &a, const quat &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

quat operator-(const quat &a, const quat &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

quat operator*(const quat &a, float b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

quat operator-(const quat &q) {
    return {-q.x, -q.y, -q.z, -q.w};
}

bool operator==(const quat &left, const quat &right) {
    return (fabsf(left.x - right.x) <= QUAT_EPSILON &&
            fabsf(left.y - right.y) <= QUAT_EPSILON &&
            fabsf(left.z - right.z) <= QUAT_EPSILON &&
            fabsf(left.w - left.w) <= QUAT_EPSILON);
}

bool operator!=(const quat &a, const quat &b) {
    return !(a == b);
}

std::ostream &operator<<(std::ostream &output, const quat &q) {
    output << "Quat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
    return output;
}

bool same_orientation(const quat &left, const quat &right) {
    return (fabsf(left.x - right.x) <= QUAT_EPSILON &&
            fabsf(left.y - right.y) <= QUAT_EPSILON &&
            fabsf(left.z - right.z) <= QUAT_EPSILON &&
            fabsf(left.w - left.w) <= QUAT_EPSILON) ||
           (fabsf(left.x + right.x) <= QUAT_EPSILON &&
            fabsf(left.y + right.y) <= QUAT_EPSILON &&
            fabsf(left.z + right.z) <= QUAT_EPSILON &&
            fabsf(left.w + left.w) <= QUAT_EPSILON);
}

float dot(const quat &a, const quat &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float len_sq(const quat &q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float len(const quat &q) {
    float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (lenSq < QUAT_EPSILON) {
        return 0.0f;
    }
    return sqrtf(lenSq);
}

void normalize(quat &q) {
    float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (lenSq < QUAT_EPSILON) {
        return;
    }
    float i_len = 1.0f / sqrtf(lenSq);

    q.x *= i_len;
    q.y *= i_len;
    q.z *= i_len;
    q.w *= i_len;
}

quat normalized(const quat &q) {
    float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (lenSq < QUAT_EPSILON) {
        return {};
    }
    float i_len = 1.0f / sqrtf(lenSq);

    return {q.x * i_len, q.y * i_len, q.z * i_len, q.w * i_len};
}

quat conjugate(const quat &q) {
    return {-q.x, -q.y, -q.z, q.w};
}

quat inverse(const quat &q) {
    float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (lenSq < QUAT_EPSILON) {
        return {};
    }
    float recip = 1.0f / lenSq;

    // conjugate / norm
    return {-q.x * recip, -q.y * recip, -q.z * recip, q.w * recip};
}
quat operator*(const quat &q1,
               const quat &q2) { // NOTE: I changed the order for compute
    return {q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
            -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
            q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
            -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w};
}

vec3 operator*(const quat &q, const vec3 &v) {
    return q.vector * 2.0f * dot(q.vector, v) +
           v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
            cross(q.vector, v) * 2.0f * q.scalar;
}

quat mix(const quat &from, const quat &to, float t) {
    return from * (1.0f - t) + to * t;
}

quat nlerp(const quat &from, const quat &to, float t) {
    return normalized(from + (to - from) * t);
}

quat operator^(const quat &q, float f) {
    float angle = 2.0f * acosf(q.scalar);
    vec3 axis = normalized(q.vector);

    float halfCos = cosf(f * angle * 0.5f);
    float halfSin = sinf(f * angle * 0.5f);

    return {axis.x * halfSin, axis.y * halfSin, axis.z * halfSin, halfCos};
}

quat slerp(const quat &start, const quat &end, float t) {
    if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON) {
        return nlerp(start, end, t);
    }

    return normalized(((inverse(start) * end) ^ t) * start);
}

quat look_rotation(const vec3 &direcion, const vec3 &up) {
    // Find orthonormal basis vectors
    vec3 f = normalized(direcion);
    vec3 u = normalized(up);
    vec3 r = cross(u, f);
    u = cross(f, r);

    // From world forward to object forward
    quat f2d = from_to(vec3(0, 0, 1), f);

    // what direction is the new object up?
    vec3 objectUp = f2d * vec3(0, 1, 0);
    // From object up to desired up
    quat u2u = from_to(objectUp, u);

    // Rotate to forward direction first, then twist to correct up
    quat result = f2d * u2u; // TODO: Need to check out. Order changed.
    // Don't forget to normalize the result
    return normalized(result);
}

mat4 quat_to_mat4(const quat &q) {
    vec3 r = q * vec3(1, 0, 0);
    vec3 u = q * vec3(0, 1, 0);
    vec3 f = q * vec3(0, 0, 1);

    return {r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0,
                1};
}

quat mat4_to_quat(const mat4 &m) {
    vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
    vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
    vec3 right = cross(up, forward);
    up = cross(forward, right);

    return look_rotation(forward, up);
}

quat quat_exp(vec3 v, float eps) {
    float halfangle = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

    if (halfangle < eps) {
        return normalized(quat(v.x, v.y, v.z, 1.0f));
    } else {
        float c = cosf(halfangle);
        float s = sinf(halfangle) / halfangle;
        return {s * v.x, s * v.y, s * v.z, c};
    }
}

vec3 to_euler(const quat &q, const std::string &order, bool degree) {
    mat4 m = quat_to_mat4(q);
    return to_euler(m, order, degree);
}

quat rotate_towards(quat from, quat to, float max_degrees_delta) {
    float num = angle(from, to);
    if (abs(num) < FLOAT_EPSILON)
        return to;
    return slerp(from, to, min(1.0f, max_degrees_delta / num));
}

float angle(quat a, quat b) {
    float num = min(abs(dot(a, b)), 1.0f);
    return (num > 0.999999f) ? 0.0f : (acos(num) * 2.0f * 57.29578f);
}
}
