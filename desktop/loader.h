#pragma once

#include "common.h"
#include "object.h"
#include "scene.h"

#include <fstream>
#include <map>
#include <vector>

bool load_materials(std::ifstream &ifs,
                    std::vector<material> &materials,
                    std::map<std::string, size_t> &material_names);
bool load_objects(std::ifstream &ifs,
                  const std::map<std::string, size_t> &material_names,
                  std::vector<object> &objects);
bool load_scene(std::ifstream &ifs, scene &scene);
