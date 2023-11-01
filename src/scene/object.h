#pragma once

#include "common.h"
#include "math3d.h"

struct face {
    array<size_t> vertex_indices;
    size_t normal_index;
    size_t material_index;
};

struct object {
    array<face> faces;
    array<m3::vec3> vertices;
    array<m3::vec3> normals;
};
