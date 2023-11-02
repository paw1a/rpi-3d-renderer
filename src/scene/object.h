#pragma once

#include "common.h"
#include "math3d.h"

struct face {
    std::vector<size_t> vertex_indices;
    size_t normal_index;
    size_t material_index;
};

struct object {
    std::vector<face> faces;
    std::vector<m3::vec3> vertices;
    std::vector<m3::vec3> normals;
};
