#include "distance.h"

#include <Eigen/Core>
#include <cmath>
#include <cstdlib>

float Distance::Euclidian(const Eigen::RowVectorXf &a, const Eigen::RowVectorXf &b) {
  return std::sqrt((a - b).array().pow(2).sum());
}

float Distance::Manhattan(const Eigen::RowVectorXf &a, const Eigen::RowVectorXf &b) {
  return a.array().absolute_difference(b.array()).sum();
}
