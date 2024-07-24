#include "classifier.h"

#include <fmt/base.h>

#include <Eigen/Core>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <map>
#include <vector>

#include "core/distance.h"

KNeighborsClassifier::KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config)
    : K_{config.K}, distanceType_{config.distanceType} {}

KNeighborsClassifier::KNeighborsClassifier() : KNeighborsClassifier(KNeighborsClassifierCreateConfig{}) {}

void KNeighborsClassifier::Fit(const Eigen::MatrixXf &x, const std::vector<int> &y) {
  assert(static_cast<size_t>(x.rows()) == y.size());

  x_ = x;
  y_ = y;
}

std::vector<int> KNeighborsClassifier::Predict(const Eigen::MatrixXf &predict) {
  std::vector<int> predictions;
  predictions.reserve(static_cast<size_t>(predict.rows()));

  for (const auto &point : predict.rowwise()) {
    auto &&neighbors = FindNeighbors(point);

    // Label -> Frequency
    std::map<int, int> labels;

    for (int i = 0; i < K_; ++i) {
      Eigen::Index idx{};
      neighbors.minCoeff(&idx);
      const int &label = y_[static_cast<size_t>(idx)];
      ++labels[label];
      neighbors(idx) = Eigen::Infinity;
    }

    const auto &mostVoted =
        *std::max_element(labels.begin(), labels.end(), [](auto l, auto r) { return l.second < r.second; });
    predictions.emplace_back(mostVoted.first);
  }

  return predictions;
}

Eigen::VectorXf KNeighborsClassifier::FindNeighbors(const Eigen::RowVectorXf &point) {
  const auto distance = [&]() {
    switch (distanceType_) {
      case kEuclidian:
        return Distance::Euclidian;
      case kManhattan:
        return Distance::Manhattan;
    }
  }();

  static int j{1};
  std::cout << "[" << j++ << "] Calculating distances from point " << point << '\n';

  return distance(point, x_);
}
