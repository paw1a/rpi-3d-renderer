#pragma once

#include "common.h"
#include "loader.h"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const point3 &point);
std::ostream &operator<<(std::ostream &os, const point2 &point);
std::ostream &operator<<(std::ostream &os, const face &face);
std::ostream &operator<<(std::ostream &os, const object &object);
std::ostream &operator<<(std::ostream &os, const polygon &polygon);
