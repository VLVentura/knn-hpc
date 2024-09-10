#include "dataset.h"

#include <fmt/format.h>

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "strings.h"

Dataset::Dataset(std::string dataset, bool test) : dataset_{std::move(dataset)}, test_{test} {
  x_ = LoadFeatures();
  y_ = LoadTarget();
}

Dataset::Dataset(std::string dataset) : Dataset(std::move(dataset), false) {}

Eigen::MatrixXf Dataset::LoadFeatures() {
  const std::string filename = test_ ? fmt::format("{}/X_test.csv", dataset_) : fmt::format("{}/X_train.csv", dataset_);
  std::ifstream features{filename};

  bool firstLine{true};
  std::string line;

  std::vector<Eigen::RowVectorXf> rows;
  while (std::getline(features, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    const auto &tokens = StringsUtils::Split(line, ',');

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

std::vector<int> Dataset::LoadTarget() {
  std::vector<int> data;
  const std::string filename = test_ ? fmt::format("{}/Y_test.csv", dataset_) : fmt::format("{}/Y_train.csv", dataset_);
  std::ifstream target{filename};

  bool firstLine{true};
  std::string line;

  while (std::getline(target, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    const int value = std::stoi(line);
    data.emplace_back(value);
  }

  target.close();
  return data;
}
