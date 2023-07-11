#pragma once

#include "common.h"
#include <fstream>
#include <map>
#include <vector>

bool load_objects(std::ifstream &ifs, std::map<std::string, object> &objects);
