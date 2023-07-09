#pragma once

#include "common.h"
#include <map>
#include <fstream>
#include <vector>

bool load_objects(std::ifstream &ifs, std::map<std::string, object> &objects);
