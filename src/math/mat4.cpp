#include "mat4.h"
#include "scalar.h"

namespace m3 {
bool operator==(const mat4 &a, const mat4 &b) {
    for (int i = 0; i < 16; ++i) {
        if (fabsf(a.v[i] - b.v[i]) > MAT4_EPSILON) {
            return false;
        }
    }
    return true;
}

bool operator!=(const mat4 &a, const mat4 &b) {
    return !(a == b);
}

mat4 operator*(const mat4 &m, float f) {
    return {m.xx * f, m.xy * f, m.xz * f, m.xw * f, m.yx * f, m.yy * f,
            m.yz * f, m.yw * f, m.zx * f, m.zy * f, m.zz * f, m.zw * f,
            m.tx * f, m.ty * f, m.tz * f, m.tw * f};
}

mat4 operator+(const mat4 &a, const mat4 &b) {
    return {a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
            a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
            a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
            a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw};
}

#define M4D(aRow, bCol)                                                        \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] +                                    \
        a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] +                                \
        a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] +                                \
        a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

mat4 operator*(const mat4 &a, const mat4 &b) {
    return {
        M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Column 0
        M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Column 1
        M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Column 2
        M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Column 3
    };
}

#define M4V4D(mRow, x, y, z, w)                                                \
    ((x)*m.v[0 * 4 + (mRow)] + (y)*m.v[1 * 4 + (mRow)] +                       \
     (z)*m.v[2 * 4 + (mRow)] + (w)*m.v[3 * 4 + (mRow)])

vec4 operator*(const mat4 &m, const vec4 &v) {
    return {M4V4D(0, v.x, v.y, v.z, v.w), M4V4D(1, v.x, v.y, v.z, v.w),
            M4V4D(2, v.x, v.y, v.z, v.w), M4V4D(3, v.x, v.y, v.z, v.w)};
}

vec3 transform_vector(const mat4 &m, const vec3 &v) {
    float a = v.v[0] * m.m[0][0] + v.v[1] * m.m[1][0] + v.v[2] * m.m[2][0] +
              m.m[3][0];
    float b = v.v[0] * m.m[0][1] + v.v[1] * m.m[1][1] + v.v[2] * m.m[2][1] +
              m.m[3][1];
    float c = v.v[0] * m.m[0][2] + v.v[1] * m.m[1][2] + v.v[2] * m.m[2][2] +
              m.m[3][2];
    float w = v.v[0] * m.m[0][3] + v.v[1] * m.m[1][3] + v.v[2] * m.m[2][3] +
              m.m[3][3];

    float one_over_w = 1.0f / w;
    return {a * one_over_w, b * one_over_w, c * one_over_w};
}

mat4 translate(const vec3 &v) {
    return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, v.x, v.y, v.z, 1};
}

mat4 scale(const vec3 &v) {
    return {v.x, 0, 0, 0, 0, v.y, 0, 0, 0, 0, v.z, 0, 0, 0, 0, 1};
}

mat4 rotate_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return {1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1};
}

mat4 rotate_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};
}

mat4 rotate_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return {c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

#define M4SWAP(x, y)                                                           \
    {                                                                          \
        float t = x;                                                           \
        x = y;                                                                 \
        y = t;                                                                 \
    }

void transpose(mat4 &m) {
    M4SWAP(m.yx, m.xy);
    M4SWAP(m.zx, m.xz);
    M4SWAP(m.tx, m.xw);
    M4SWAP(m.zy, m.yz);
    M4SWAP(m.ty, m.yw);
    M4SWAP(m.tz, m.zw);
}

mat4 transposed(const mat4 &m) {
    return {m.xx, m.yx, m.zx, m.tx, m.xy, m.yy, m.zy, m.ty,
            m.xz, m.yz, m.zz, m.tz, m.xw, m.yw, m.zw, m.tw};
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2)                                    \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] -                 \
                         m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) -                \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] -                 \
                         m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) +                \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] -                 \
                         m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float determinant(const mat4 &m) {
    return m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3) -
           m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3) +
           m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3) -
           m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

mat4 adjugate(const mat4 &m) {
    // Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
    mat4 cofactor;

    cofactor.v[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
    cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
    cofactor.v[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
    cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

    cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
    cofactor.v[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
    cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
    cofactor.v[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

    cofactor.v[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
    cofactor.v[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
    cofactor.v[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
    cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

    cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
    cofactor.v[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
    cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
    cofactor.v[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

    return transposed(cofactor);
}

std::ostream &operator<<(std::ostream &output, const mat4 &m) {
    output << "Mat4(" << m.xx << "," << m.xy << "," << m.xz << "," << m.xw
           << std::endl
           << m.yx << "," << m.yy << "," << m.yz << "," << m.yw << std::endl
           << m.zx << "," << m.zy << "," << m.zz << "," << m.zw << std::endl
           << m.tx << "," << m.ty << "," << m.tz << "," << m.tw << std::endl
           << ")";
    return output;
}

mat4 inverse(const mat4 &m) {
    float det = determinant(m);

    if (det == 0.0f) { // Epsilon check would need to be REALLY small
        std::cout
            << "WARNING: Trying to invert a matrix with a zero determinant\n";
        return {};
    }
    mat4 adj = adjugate(m);

    return adj * (1.0f / det);
}

void invert(mat4 &m) {
    float det = determinant(m);

    if (det == 0.0f) {
        std::cout
            << "WARNING: Trying to invert a matrix with a zero determinant\n";
        m = mat4();
        return;
    }

    m = adjugate(m) * (1.0f / det);
}

mat4 frustum(float l, float r, float b, float t, float n, float f) {
    if (l == r || t == b || n == f) {
        std::cout << "WARNING: Trying to create invalid frustum\n";
        return {}; // Error
    }
    return {(2.0f * n) / (r - l),
            0,
            0,
            0,
            0,
            (2.0f * n) / (t - b),
            0,
            0,
            (r + l) / (r - l),
            (t + b) / (t - b),
            (-(f + n)) / (f - n),
            1,
            0,
            0,
            (-2 * f * n) / (f - n),
            0};
}

mat4 perspective(float fov, float aspect, float znear, float zfar) {
    float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
    float xmax = ymax * aspect;

    return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 ortho(float l, float r, float b, float t, float n, float f) {
    if (l == r || t == b || n == f) {
        return {}; // Error
    }
    return {2.0f / (r - l),
            0,
            0,
            0,
            0,
            2.0f / (t - b),
            0,
            0,
            0,
            0,
            -2.0f / (f - n),
            0,
            -((r + l) / (r - l)),
            -((t + b) / (t - b)),
            -((f + n) / (f - n)),
            1};
}

mat4 look_at(const vec3 &position, const vec3 &target, const vec3 &up) {
    // Remember, forward is negative z
    vec3 f = normalized(target - position) * -1.0f;
    vec3 r = cross(up, f); // Right handed
    if (r == vec3(0, 0, 0)) {
        return {}; // Error
    }
    normalize(r);
    vec3 u = normalized(cross(f, r)); // Right handed

    vec3 t = vec3(-dot(r, position), -dot(u, position), -dot(f, position));

    return {// Transpose upper 3x3 matrix to invert it
            r.x, u.x, f.x, 0, r.y, u.y, f.y, 0,
            r.z, u.z, f.z, 0, t.x, t.y, t.z, 1};
}

vec3 to_euler(const mat4 &m, const std::string &order, bool degree) {
    double a = m.xx;
    double f = m.yx;
    float g = m.zx;
    double h = m.xy;
    double k = m.yy;
    float l = m.zy;
    double s = m.xz;
    double n = m.yz;
    double e = m.zz;

    vec3 euler = {};
    float &x = euler.x;
    float &y = euler.y;
    float &z = euler.z;

    if ("XYZ" == order) {
        y = std::asin(clamp(g, -1, 1));

        if (0.999999 > std::abs(g)) {
            x = std::atan2(-l, e);
            z = std::atan2(-f, a);
        } else {
            x = std::atan2(n, k);
            z = 0;
        }
    } else if ("YXZ" == order) {
        x = std::asin(-clamp(l, -1, 1));

        if (0.999999 > std::abs(l)) {
            y = std::atan2(g, e);
            z = std::atan2(h, k);
        } else {
            y = std::atan2(-s, a);
            z = 0;
        }
    } else if ("ZXY" == order) {
        x = std::asin(clamp(n, -1, 1));

        if (0.999999 > std::abs(n)) {
            y = std::atan2(-s, e);
            z = std::atan2(-f, k);

        } else {
            y = std::atan2(h, a);
            z = 0;
        }
    } else if ("ZYX" == order) {
        y = std::asin(-clamp(s, -1, 1));

        if (0.999999 > abs(s)) {
            x = std::atan2(n, e);
            z = std::atan2(h, a);
        } else {
            x = 0;
            z = std::atan2(-f, k);
        }
    } else if ("YZX" == order) {
        z = std::asin(clamp(h, -1, 1));

        if (0.999999 > abs(h)) {
            x = std::atan2(-l, k);
            y = std::atan2(-s, a);

        } else {
            y = std::atan2(g, e);
            x = 0;
        }
    } else if ("XZY" == order) {
        z = std::asin(-clamp(f, -1, 1));

        if (0.999999 > abs(f)) {
            x = std::atan2(n, k);
            y = std::atan2(g, a);
        } else {
            x = std::atan2(-l, e);
            y = 0;
        }
    }

    if (degree) {
        x = rad2deg(x);
        y = rad2deg(y);
        z = rad2deg(z);
    }

    return euler;
}
}
