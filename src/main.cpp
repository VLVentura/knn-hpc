#include <fmt/base.h>

#include <Eigen/Dense>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "core/distance.h"
#include "core/vec4.h"
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

std::vector<Vec4> LoadFeatures(const std::string &filename) {
  std::vector<Vec4> data;
  std::ifstream features{filename};

  bool firstLine{true};
  std::string line;

  while (std::getline(features, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }
    fmt::println("line: {}", line);
    Vec4 point{};
    const auto &tokens = SplitString(line, ',');
    point.x = std::stof(tokens[0]);
    point.y = std::stof(tokens[1]);
    point.w = std::stof(tokens[2]);
    point.z = std::stof(tokens[3]);

    data.emplace_back(point);
  }

  features.close();
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
    fmt::println("{} | {} | {}", predict[i], y[i], right);
  }

  const float acc = static_cast<float>(right) / static_cast<float>(predict.size());
  fmt::println("Accuracy: {:.2f}%", acc * 100.F);  // NOLINT

  return EXIT_SUCCESS;
}
