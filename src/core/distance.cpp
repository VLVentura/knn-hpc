#include "distance.h"

#include <fmt/base.h>

#include <cmath>
#include <cstddef>
#include <cstdlib>

Vector Distance::Euclidian(const Vector &a, const Matrix &b) {
  Vector distances(b.size());

  for (size_t i = 0; i < b.size(); ++i) {
    float sum{0.0F};

    const auto &row = b[i];
    for (size_t j = 0; j < row.size(); ++j) {
      sum += static_cast<float>(std::pow((a[j] - row[j]), 2));
    }

    distances[i] = std::sqrt(sum);
  }

  return distances;
}

Vector Distance::Manhattan(const Vector &a, [[maybe_unused]] const Matrix &b) { return a; }
