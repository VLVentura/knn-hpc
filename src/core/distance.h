#ifndef CORE_DISTANCE_H
#define CORE_DISTANCE_H

#include "core/vec4.h"
// #include "vec2.h"

enum DistanceType {
  kEuclidian,
  kManhattan,
};

class Distance {
public:
  // static float Euclidian(const Vec2 &a, const Vec2 &b);
  // static float Manhattan(const Vec2 &a, const Vec2 &b);
  static float Euclidian(const Vec4 &a, const Vec4 &b);
  static float Manhattan(const Vec4 &a, const Vec4 &b);
};

#endif  // CORE_DISTANCE_H
