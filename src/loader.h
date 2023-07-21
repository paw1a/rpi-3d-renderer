#pragma once

#include "common.h"
#include "object.h"
#include "scene.h"

#include <fstream>
#include <map>
#include <vector>

char *read_file(const char *filename);
bool load_materials(std::istream &is, std::vector<material> &materials,
                    std::map<std::string, size_t> &material_names);
bool load_objects(std::istream &is,
                  const std::map<std::string, size_t> &material_names,
                  std::vector<object> &objects);
bool load_scene(std::istream &is, scene &scene);
