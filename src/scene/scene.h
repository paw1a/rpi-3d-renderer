#pragma once

#include "object.h"

#include <map>
#include <vector>

struct camera {
    m3::vec3 position;
    m3::vec3 target;
    m3::vec3 up;
};

struct scene {
    std::vector<object> objects;
    std::vector<material> materials;
    std::vector<m3::vec3> lights;
    struct camera camera;
};
