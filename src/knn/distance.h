#ifndef CORE_DISTANCE_H
#define CORE_DISTANCE_H

#include <Eigen/Core>

enum DistanceType {
  kEuclidian,
  kManhattan,
};

class Distance {
public:
  static Eigen::VectorXf Euclidian(const Eigen::RowVectorXf &a, const Eigen::MatrixXf &b);
  static Eigen::VectorXf Manhattan(const Eigen::RowVectorXf &a, const Eigen::MatrixXf &b);
};

#endif  // CORE_DISTANCE_H
