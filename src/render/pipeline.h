#pragma once

#include "common.h"
#include "object.h"
#include "scene.h"

#include <map>
#include <vector>

bool scene_to_polygons(const scene &scene, array<polygon> &polygons);
