#pragma once

#include "object.h"

#include <map>

struct camera {
    m3::vec3 position;
    m3::vec3 target;
    m3::vec3 up;
};

struct scene {
    array<object> objects;
    array<material> materials;
    array<m3::vec3> lights;
    struct camera camera;
};
