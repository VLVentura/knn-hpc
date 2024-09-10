#include "classifier.h"

#include <fmt/base.h>
#include <fmt/format.h>
#include <omp.h>
#include <spdlog/spdlog.h>

#include <Eigen/Core>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <map>
#include <vector>

#include "distance.h"

KNeighborsClassifier::KNeighborsClassifier(const KNeighborsClassifierCreateConfig &config)
    : K_{config.K},
      jobs_{config.jobs},
      distanceType_{config.distanceType},
      startLine_{config.startLine},
      endLine_{config.endLine},
      outputFile_{config.outputFile} {}

KNeighborsClassifier::KNeighborsClassifier() : KNeighborsClassifier(KNeighborsClassifierCreateConfig{}) {}

void KNeighborsClassifier::Fit(const Eigen::MatrixXf &x, const std::vector<int> &y) {
  assert(static_cast<size_t>(x.rows()) == y.size());

  x_ = x;
  y_ = y;
}

void KNeighborsClassifier::Predict(const Eigen::MatrixXf &predict) {
  static const char *const EnablePrintCalc = getenv("ENABLE_PRINT_CALC");       // NOLINT
  static const Eigen::IOFormat EigenFmt(2, 0, ", ", "\n", "[", "]", "[", "]");  // NOLINT

  std::vector<int> predictions(static_cast<size_t>(predict.rows()));
  int64_t i{0};

  omp_set_num_threads(jobs_);

/**
  - Schedule static: independente da carga na thread.
  - Schedule dynamic: depende da carga, se houver um overhead grande em uma thread, a outra thread pode suprir.
 */
#pragma omp parallel for schedule(static, 1) default(shared) private(i)  // NOLINT
  for (i = startLine_; i < endLine_; ++i) {
    const auto &point = predict.row(i);
    auto &&neighbors = FindNeighbors(point);

    if (EnablePrintCalc != nullptr) {
      std::stringstream ss;
      ss << point.format(EigenFmt);
      spdlog::info("[Thread : {}] | [{}] Calculating distances from point {}", omp_get_thread_num(), i, ss.str());
    }

    // Label -> Frequency
    std::map<int, int> labels;

    for (int j = 0; j < K_; ++j) {
      Eigen::Index idx{0};
      neighbors.minCoeff(&idx);
      const int &label = y_[static_cast<size_t>(idx)];
      ++labels[label];
      neighbors(idx) = Eigen::Infinity;
    }

    const auto &mostVoted =
        *std::max_element(labels.begin(), labels.end(), [](auto l, auto r) { return l.second < r.second; });

    predictions[static_cast<size_t>(i)] = mostVoted.first;
  }

  std::fstream file{outputFile_, std::ios::out};
  file << fmt::format("{},{}\n", startLine_, endLine_);
  for (i = startLine_; i < endLine_; ++i) {
    file << predictions[static_cast<size_t>(i)] << '\n';
  }
  file.close();
}

Eigen::VectorXf KNeighborsClassifier::FindNeighbors(const Eigen::RowVectorXf &point) {
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
