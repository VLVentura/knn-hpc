#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <queue>
#include <vector>

#include "core/distance.h"
#include "core/vec4.h"

struct KNeighborsClassifierCreateConfig {
  int K{3};  // NOLINT
  DistanceType distanceType{DistanceType::kEuclidian};
};

struct DistanceIndex {
  DistanceIndex(float d, int i) : distance{d}, index{i} {}
  [[nodiscard]] inline bool operator<(const DistanceIndex &rhs) const { return distance < rhs.distance; }

  float distance;
  int index;
};

class KNeighborsClassifier {
public:
  explicit KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config);
  KNeighborsClassifier();

  void Fit(const std::vector<Vec4> &x, const std::vector<int> &y);
  std::vector<int> Predict(const std::vector<Vec4> &predict);

private:
  std::priority_queue<DistanceIndex> FindNeighbors(const Vec4 &point);

  int K_;  // NOLINT
  DistanceType distanceType_;
  std::vector<Vec4> x_;
  std::vector<int> y_;
};

#endif  // KNN_CLASSIFIER_H
