#include "distance.h"

#include <Eigen/src/Core/Matrix.h>

#include <Eigen/Core>
#include <cmath>
#include <cstdlib>

Eigen::VectorXf Distance::Euclidian(const Eigen::RowVectorXf &a, const Eigen::MatrixXf &b) {
  return (b.rowwise() - a).rowwise().squaredNorm();
}

Eigen::VectorXf Distance::Manhattan(const Eigen::RowVectorXf &a, const Eigen::MatrixXf &b) { return a + b.row(0); }
