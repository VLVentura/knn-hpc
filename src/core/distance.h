#ifndef CORE_DISTANCE_H
#define CORE_DISTANCE_H

#include <Eigen/Core>

enum DistanceType {
  kEuclidian,
  kManhattan,
};

class Distance {
public:
  static float Euclidian(const Eigen::RowVectorXf &a, const Eigen::RowVectorXf &b);
  static float Manhattan(const Eigen::RowVectorXf &a, const Eigen::RowVectorXf &b);
};

#endif  // CORE_DISTANCE_H
