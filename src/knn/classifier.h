#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <Eigen/Core>
#include <queue>
#include <vector>

#include "core/distance.h"

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

  void Fit(const Eigen::MatrixXf &x, const std::vector<int> &y);
  std::vector<int> Predict(const Eigen::MatrixXf &predict);

private:
  std::priority_queue<DistanceIndex> FindNeighbors(const Eigen::RowVectorXf &point);

  int K_;  // NOLINT
  DistanceType distanceType_;
  Eigen::MatrixXf x_;
  std::vector<int> y_;
};

#endif  // KNN_CLASSIFIER_H
