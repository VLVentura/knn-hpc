#ifndef CORE_DISTANCE_H
#define CORE_DISTANCE_H

#include "utils.h"

enum DistanceType {
  kEuclidian,
  kManhattan,
};

class Distance {
public:
  static Vector Euclidian(const Vector &a, const Matrix &b);
  static Vector Manhattan(const Vector &a, const Matrix &b);
};

#endif  // CORE_DISTANCE_H
