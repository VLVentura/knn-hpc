#include "distance.h"

#include <cmath>
#include <cstdlib>

#include "vec4.h"

// #include "vec2.h"

// float Distance::Euclidian(const Vec2 &a, const Vec2 &b) {
//   const auto x = static_cast<float>(std::pow(a.x - b.x, 2));
//   const auto y = static_cast<float>(std::pow(a.y - b.y, 2));
//   return std::sqrt(x + y);
// }

// float Distance::Manhattan(const Vec2 &a, const Vec2 &b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); }

float Distance::Euclidian(const Vec4 &a, const Vec4 &b) {
  const auto x = static_cast<float>(std::pow(a.x - b.x, 2));
  const auto y = static_cast<float>(std::pow(a.y - b.y, 2));
  const auto w = static_cast<float>(std::pow(a.w - b.w, 2));
  const auto z = static_cast<float>(std::pow(a.z - b.z, 2));
  return std::sqrt(x + y + w + z);
}

float Distance::Manhattan(const Vec4 &a, const Vec4 &b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.w - b.w) + std::abs(a.z - b.z);
}
