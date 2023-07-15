#pragma once

#include "common.h"
#include <fstream>
#include <map>
#include <vector>

bool load_materials(std::ifstream &ifs,
                    std::map<std::string, material> &materials);
bool load_objects(std::ifstream &ifs,
                  const std::map<std::string, material> &materials,
                  std::map<std::string, object> &objects);
