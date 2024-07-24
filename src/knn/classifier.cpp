#include "classifier.h"

#include <fmt/base.h>

#include <Eigen/Core>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <queue>
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
      const auto &n = neighbors.top();
      const int &label = y_[static_cast<size_t>(n.index)];
      ++labels[label];
      neighbors.pop();
    }

    const auto &mostVoted =
        *std::max_element(labels.begin(), labels.end(), [](auto l, auto r) { return l.second < r.second; });
    predictions.emplace_back(mostVoted.first);
  }

  return predictions;
}

std::priority_queue<DistanceIndex> KNeighborsClassifier::FindNeighbors(const Eigen::RowVectorXf &point) {
  const auto distance = [&]() {
    switch (distanceType_) {
      case kEuclidian:
        return Distance::Euclidian;
      case kManhattan:
        return Distance::Manhattan;
    }
  }();

  std::priority_queue<DistanceIndex> distances;
  for (Eigen::Index i = 0; i < x_.rows(); ++i) {
    const float &d = distance(point, x_.row(i));
    distances.emplace(-d, i);
  }

  return distances;
}
