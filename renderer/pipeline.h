#pragma once

#include "common.h"
#include <map>
#include <vector>

bool preprocess_objects(const std::map<std::string, object> &objects,
                        const std::vector<m3::vec3> &lights,
                        std::vector<polygon> &polygons);
void adjust_data_to_display(std::map<std::string, object> &objects);
