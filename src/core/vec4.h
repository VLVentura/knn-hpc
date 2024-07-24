
#ifndef CORE_VEC4_H
#define CORE_VEC4_H

#include <fmt/ostream.h>

#include <ostream>

#include "hash.h"

struct Vec4 {
  inline Vec4& operator-(const Vec4& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    w -= rhs.w;
    z -= rhs.z;
    return *this;
  }

  inline Vec4& operator+(const Vec4& rhs) {
    x += rhs.x;
    y += rhs.y;
    w += rhs.w;
    z += rhs.z;
    return *this;
  }

  // Overload the << operator for the Point class
  friend std::ostream& operator<<(std::ostream& os, const Vec4& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.w << ", " << vec.z << ")";
    return os;
  }

  [[nodiscard]] inline bool operator==(const Vec4& rhs) const {
    return x == rhs.x && y == rhs.y && w == rhs.w && z == rhs.z;
  }

  [[nodiscard]] inline bool operator<(const Vec4& rhs) const {
    return x < rhs.x && y < rhs.y && w < rhs.w && z < rhs.z;
  }

  float x, y, w, z;
};

template <>
struct fmt::formatter<Vec4> : ostream_formatter {};

// https://stackoverflow.com/questions/30149697/defining-hash-function-as-part-of-a-struct
namespace std {

template <>
struct hash<Vec4> {
  size_t operator()(const Vec4& vec) const {
    size_t seed{0};
    HashCombine(seed, vec.x, vec.y, vec.w, vec.z);
    return seed;
  }
};

}  // namespace std

#endif  // CORE_VEC4_H
