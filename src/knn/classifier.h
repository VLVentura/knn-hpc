#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <vector>

#include "core/distance.h"
#include "core/utils.h"

struct KNeighborsClassifierCreateConfig {
  int K{3};  // NOLINT
  DistanceType distanceType{DistanceType::kEuclidian};
  int jobs{1};
};

class KNeighborsClassifier {
public:
  explicit KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config);
  KNeighborsClassifier();

  void Fit(const Matrix &x, const std::vector<int> &y);
  std::vector<int> Predict(const Matrix &predict);

private:
  Vector FindNeighbors(const Vector &point);

  int K_;  // NOLINT
  int jobs_;
  DistanceType distanceType_;
  Matrix x_;
  std::vector<int> y_;
};

#endif  // KNN_CLASSIFIER_H
