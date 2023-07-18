#include "transform.h"
#include <cmath>

namespace m3 {
transform combine(const transform &parent, const transform &curr) {
    transform out;

    out.scale = parent.scale * curr.scale;
    out.rotation = parent.rotation * curr.rotation;
    out.position =
        parent.position + parent.rotation * (parent.scale * curr.position);

    return out;
}

transform operator*(const transform &parent, const transform &curr) {
    return combine(parent, curr);
}

transform inverse(const transform &t) {
    transform inv;

    inv.rotation = inverse(t.rotation);

    inv.scale.x = fabs(t.scale.x) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.x;
    inv.scale.y = fabs(t.scale.y) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.y;
    inv.scale.z = fabs(t.scale.z) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.z;

    vec3 invTranslation = t.position * -1.0f;
    inv.position = inv.rotation * (inv.scale * invTranslation);

    return inv;
}

transform mix(const transform &a, const transform &b, float t) {
    quat bRot = b.rotation;
    if (dot(a.rotation, bRot) < 0.0f) {
        bRot = -bRot;
    }
    return {lerp(a.position, b.position, t), nlerp(a.rotation, bRot, t),
            lerp(a.scale, b.scale, t)};
}

bool operator==(const transform &a, const transform &b) {
    return a.position == b.position && a.rotation == b.rotation &&
           a.scale == b.scale;
}

bool operator!=(const transform &a, const transform &b) {
    return !(a == b);
}

mat4 transform_to_mat4(const transform &t) {
    // First, extract the rotation basis of the transform
    vec3 x = t.rotation * vec3(1, 0, 0);
    vec3 y = t.rotation * vec3(0, 1, 0);
    vec3 z = t.rotation * vec3(0, 0, 1);

    // Next, scale the basis vectors
    x = x * t.scale.x;
    y = y * t.scale.y;
    z = z * t.scale.z;

    // Extract the position of the transform
    vec3 p = t.position;

    // Create matrix
    return mat4{
        x.x, x.y, x.z, 0, // X basis (& Scale)
        y.x, y.y, y.z, 0, // Y basis (& scale)
        z.x, z.y, z.z, 0, // Z basis (& scale)
        p.x, p.y, p.z, 1  // Position
    };
}

transform mat4_to_transform(const mat4 &m) {
    transform out;

    out.position = vec3(m.v[12], m.v[13], m.v[14]);
    out.rotation = mat4_to_quat(m);

    mat4 rotScaleMat(m.v[0], m.v[1], m.v[2], 0, m.v[4], m.v[5], m.v[6], 0,
                     m.v[8], m.v[9], m.v[10], 0, 0, 0, 0, 1);
    mat4 invRotMat = quat_to_mat4(inverse(out.rotation));
    mat4 scaleSkewMat = rotScaleMat * invRotMat;

    out.scale = vec3(scaleSkewMat.v[0], scaleSkewMat.v[5], scaleSkewMat.v[10]);

    return out;
}

vec3 transform_point(const transform &a, const vec3 &b) {
    vec3 out;

    out = a.rotation * (a.scale * b);
    out = a.position + out;

    return out;
}

vec3 transform_vector(const transform &a, const vec3 &b) {
    vec3 out;

    out = a.rotation * (a.scale * b);

    return out;
}
} // namespace m3
