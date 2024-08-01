#include "classifier.h"

#include <fmt/base.h>
#include <fmt/ranges.h>
#include <omp.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <queue>
#include <vector>

#include "core/distance.h"

KNeighborsClassifier::KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config)
    : K_{config.K}, jobs_{config.jobs}, distanceType_{config.distanceType} {}

KNeighborsClassifier::KNeighborsClassifier() : KNeighborsClassifier(KNeighborsClassifierCreateConfig{}) {}

void KNeighborsClassifier::Fit(const Matrix &x, const std::vector<int> &y) {
  assert(x.size() == y.size());

  x_ = x;
  y_ = y;
}

std::vector<int> KNeighborsClassifier::Predict(const Matrix &predict) {
  static const char *const EnableVerbose = getenv("ENABLE_VERBOSE");  // NOLINT

  std::vector<int> predictions(predict.size());
  size_t i{0};

  omp_set_num_threads(jobs_);

/**
  - Schedule static: independente da carga na thread.
  - Schedule dynamic: depende da carga, se houver um overhead grande em uma thread, a outra thread pode suprir.
 */
#pragma omp parallel for schedule(static, 1) default(shared) private(i)  // NOLINT
  for (i = 0; i < predict.size(); ++i) {
    const auto &point = predict[i];
    auto &&neighbors = FindNeighbors(point);

    if (EnableVerbose != nullptr) {
      spdlog::info("[Thread : {}] | [{}] Calculating distances from point {}", omp_get_thread_num(), i,
                   fmt::join(point, ","));
    }

    struct DistanceIndex {
      DistanceIndex(float d, size_t s) : distance{d}, index{s} {}
      [[nodiscard]] inline bool operator<(const DistanceIndex &rhs) const { return distance < rhs.distance; }

      float distance;
      size_t index;
    };

    std::priority_queue<DistanceIndex> distances;
    for (size_t j = 0; j < neighbors.size(); ++j) {
      distances.emplace(-neighbors[j], j);
    }

    // Label -> Frequency
    std::map<int, int> labels;

    for (size_t j = 0; j < static_cast<size_t>(K_); ++j) {
      const auto &n = distances.top();
      const int &label = y_[n.index];
      ++labels[label];
      distances.pop();
    }

    const auto &mostVoted =
        *std::max_element(labels.begin(), labels.end(), [](auto l, auto r) { return l.second < r.second; });

    predictions[i] = mostVoted.first;
  }

  return predictions;
}

Vector KNeighborsClassifier::FindNeighbors(const Vector &point) {
  const auto distance = [&]() {
    switch (distanceType_) {
      case kEuclidian:
        return Distance::Euclidian;
      case kManhattan:
        return Distance::Manhattan;
      default:
        return Distance::Euclidian;
    }
  }();

  return distance(point, x_);
}
