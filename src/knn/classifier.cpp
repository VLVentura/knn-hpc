#include "classifier.h"

#include <fmt/base.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <queue>
#include <vector>

#include "core/distance.h"
#include "core/vec4.h"

KNeighborsClassifier::KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config)
    : K_{config.K}, distanceType_{config.distanceType} {}

KNeighborsClassifier::KNeighborsClassifier() : KNeighborsClassifier(KNeighborsClassifierCreateConfig{}) {}

void KNeighborsClassifier::Fit(const std::vector<Vec4> &x, const std::vector<int> &y) {
  assert(x.size() == y.size());

  x_ = x;
  y_ = y;
}

std::vector<int> KNeighborsClassifier::Predict(const std::vector<Vec4> &predict) {
  std::vector<int> predictions;
  predictions.reserve(predict.size());

  for (const Vec4 &point : predict) {
    auto &&neighbors = FindNeighbors(point);

    // Label -> Frequency
    std::map<int, int> labels;

    for (int i = 0; i < K_; ++i) {
      const auto &n = neighbors.top();
      const int &label = y_[static_cast<size_t>(n.index)];
      ++labels[label];
      fmt::println("d: {} | i: {} | label: {} | freq: {}", n.distance, n.index, label, labels[label]);
      neighbors.pop();
    }

    const auto &mostVoted =
        *std::max_element(labels.begin(), labels.end(), [](auto l, auto r) { return l.second < r.second; });
    predictions.emplace_back(mostVoted.first);
  }

  return predictions;
}

std::priority_queue<DistanceIndex> KNeighborsClassifier::FindNeighbors(const Vec4 &point) {
  const auto distance = [&]() {
    switch (distanceType_) {
      case kEuclidian:
        return Distance::Euclidian;
      case kManhattan:
        return Distance::Manhattan;
    }
  }();

  std::priority_queue<DistanceIndex> distances;
  fmt::println("distances from {}: [ ", point);
  for (size_t i = 0; i < x_.size(); ++i) {  // NOLINT
    const Vec4 &p = x_.at(i);
    const float &d = distance(point, p);
    fmt::println("\t{}: {}, ", p, d);
    distances.emplace(-d, i);
  }
  fmt::println(" ] | top: {}", distances.top().distance);

  return distances;
}
