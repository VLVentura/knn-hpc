#include <fmt/base.h>

#include <Eigen/Core>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "core/distance.h"
#include "knn/classifier.h"

std::vector<std::string> SplitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss{str};

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

Eigen::MatrixXf LoadFeatures(const std::string &filename) {
  std::ifstream features{filename};

  bool firstLine{true};
  std::string line;

  std::vector<Eigen::RowVectorXf> rows;
  while (std::getline(features, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    const auto &tokens = SplitString(line, ',');

    Eigen::RowVectorXf row{tokens.size()};
    for (size_t i{0}; i < tokens.size(); ++i) {
      const auto &idx = static_cast<Eigen::Index>(i);
      row(idx) = std::stof(tokens[i]);
    }

    rows.emplace_back(row);
  }
  features.close();

  Eigen::MatrixXf data{rows.size(), rows[0].size()};
  for (size_t i{0}; i < rows.size(); ++i) {
    const auto &idx = static_cast<Eigen::Index>(i);
    data.row(idx) = rows[i];
  }
  return data;
}

std::vector<int> LoadTarget(const std::string &filename) {
  std::vector<int> data;
  std::ifstream features{filename};

  bool firstLine{true};
  std::string line;

  while (std::getline(features, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    const int value = std::stoi(line);
    data.emplace_back(value);
  }

  features.close();
  return data;
}

int main() {
  const auto &xTrain = LoadFeatures("resources/iris_dataset/X_train.csv");
  const auto &yTtrain = LoadTarget("resources/iris_dataset/Y_train.csv");
  const auto &x = LoadFeatures("resources/iris_dataset/X_test.csv");
  const auto &y = LoadTarget("resources/iris_dataset/Y_test.csv");

  KNeighborsClassifierCreateConfig config{};
  config.K = 5;  // NOLINT
  config.distanceType = DistanceType::kEuclidian;

  KNeighborsClassifier knn{config};
  knn.Fit(xTrain, yTtrain);
  const auto &predict = knn.Predict(x);

  size_t right{0};
  for (size_t i = 0; i < predict.size(); ++i) {
    right = (predict[i] == y[i]) ? right + 1 : right;
  }

  const float acc = static_cast<float>(right) / static_cast<float>(predict.size());
  fmt::println("Accuracy: {:.2f}%", acc * 100.F);  // NOLINT

  return EXIT_SUCCESS;
}
