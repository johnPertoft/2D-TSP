#ifndef UTIL_H
#define UTIL_H

#include <cmath>

struct vertex {
  double x, y;
};

double distance(vertex &a, vertex &b) {
  double x = a.x - b.x;
  double y = a.y - b.y;
  return std::rint(sqrt(x*x + y*y));
}

#endif
