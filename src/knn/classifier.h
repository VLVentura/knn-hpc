#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <Eigen/Core>
#include <string>
#include <vector>

#include "distance.h"

struct KNeighborsClassifierCreateConfig {
  int K{3};  // NOLINT
  DistanceType distanceType{DistanceType::kEuclidian};
  int jobs{1};
  int startLine{0};
  int endLine{0};
  std::string outputFile;
};

class KNeighborsClassifier {
public:
  explicit KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config);
  KNeighborsClassifier();

  void Fit(const Eigen::MatrixXf &x, const std::vector<int> &y);
  void Predict(const Eigen::MatrixXf &predict);

private:
  Eigen::VectorXf FindNeighbors(const Eigen::RowVectorXf &point);

  int K_;  // NOLINT
  int jobs_;
  DistanceType distanceType_;
  int startLine_;
  int endLine_;
  std::string outputFile_;

  Eigen::MatrixXf x_;
  std::vector<int> y_;
};

#endif  // KNN_CLASSIFIER_H
