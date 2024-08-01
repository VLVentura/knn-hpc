#include <fmt/base.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <CLI/CLI.hpp>
#include <Eigen/Core>
#include <cstdlib>
#include <fstream>
#include <map>
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

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char **argv) {
  CLI::App app{"KNN Algorithm implemented with HPC techniques."};
  argv = app.ensure_utf8(argv);

  std::string dataset{"resources/iris_dataset"};
  app.add_option("-d,--dataset", dataset)->check(CLI::ExistingPath);

  int K{3};                                                        // NOLINT
  app.add_option("-k,--neighbors", K)->check(CLI::Range(3, 100));  // NOLINT

  DistanceType distance{DistanceType::kEuclidian};
  const std::map<std::string, DistanceType> map{{"euclidian", DistanceType::kEuclidian},
                                                {"e", DistanceType::kEuclidian},
                                                {"manhattan", DistanceType::kManhattan},
                                                {"m", DistanceType::kManhattan}};
  app.add_option("-f,--distance", distance)
      ->check(CLI::IsMember({"euclidian", "e", "manhattan", "m"}, CLI::ignore_case))
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case));

  int jobs{1};
  app.add_option("-j,--jobs", jobs);

  bool verbose{false};
  app.add_flag("-v,--verbose", verbose);

  CLI11_PARSE(app, argc, argv);

  if (verbose) {
    setenv("ENABLE_VERBOSE", "true", 0);  // NOLINT
  }

  const auto &xTrain = LoadFeatures(fmt::format("{}/X_train.csv", dataset));
  const auto &yTtrain = LoadTarget(fmt::format("{}/Y_train.csv", dataset));
  const auto &x = LoadFeatures(fmt::format("{}/X_test.csv", dataset));
  const auto &y = LoadTarget(fmt::format("{}/Y_test.csv", dataset));

  KNeighborsClassifierCreateConfig config{};
  config.K = K;  // NOLINT
  config.distanceType = distance;
  config.jobs = jobs;

  const spdlog::stopwatch sw;
  KNeighborsClassifier knn{config};
  knn.Fit(xTrain, yTtrain);
  const auto &predict = knn.Predict(x);
  const auto &elapsed = sw.elapsed();

  size_t right{0};
  for (size_t i = 0; i < predict.size(); ++i) {
    right = (predict[i] == y[i]) ? right + 1 : right;
  }

  const float acc = static_cast<float>(right) / static_cast<float>(predict.size());
  if (verbose) {
    spdlog::info("Accuracy: {:.2f}% | Time: {}", acc * 100.F, elapsed.count());  // NOLINT
  } else {
    fmt::println("{:.2f} | {}", acc, elapsed.count());
  }

  return EXIT_SUCCESS;
}
