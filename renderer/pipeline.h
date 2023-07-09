#pragma once

#include "common.h"
#include <map>
#include <vector>

int preprocess_objects(const std::map<std::string, object> &objects, std::vector<polygon> &polygons);
