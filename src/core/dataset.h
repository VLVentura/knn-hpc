#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include <Eigen/Core>
#include <string>
#include <vector>

class Dataset {
public:
  explicit Dataset(std::string dataset, bool test);
  explicit Dataset(std::string dataset);

  [[nodiscard]] inline const Eigen::MatrixXf& X() const { return x_; }
  [[nodiscard]] inline const std::vector<int>& Y() const { return y_; }

private:
  Eigen::MatrixXf LoadFeatures();
  std::vector<int> LoadTarget();

  std::string dataset_;
  bool test_;

  Eigen::MatrixXf x_;
  std::vector<int> y_;
};

#endif  // CORE_DATASET_H
