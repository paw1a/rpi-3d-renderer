#pragma once

#include "common.h"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const point2 &point);
std::ostream &operator<<(std::ostream &os, const color &color);
std::ostream &operator<<(std::ostream &os, const material &material);
std::ostream &operator<<(std::ostream &os, const face &face);
std::ostream &operator<<(std::ostream &os, const object &object);
std::ostream &operator<<(std::ostream &os, const polygon &polygon);
std::ostream &operator<<(std::ostream &os, const window &window);
