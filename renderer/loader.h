#pragma once

#include "common.h"
#include <map>
#include <iostream>
#include <vector>

struct face {
    std::vector<size_t> vertex_indices;
    size_t normal_index;
    color color;
};

struct object {
    std::vector<face> faces;
    std::vector<point3> vertices;
    std::vector<vec3> normals;
};

bool load_objects(std::ifstream &ifs, std::map<std::string, object> &objects);
