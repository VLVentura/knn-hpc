
#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include "hash.h"

struct Vec2 {
  inline Vec2& operator-(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  inline Vec2& operator+(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  [[nodiscard]] inline bool operator==(const Vec2& rhs) const { return x == rhs.x && y == rhs.y; }

  [[nodiscard]] inline bool operator<(const Vec2& rhs) const { return x < rhs.x && y < rhs.y; }

  float x, y;
};

// https://stackoverflow.com/questions/30149697/defining-hash-function-as-part-of-a-struct
namespace std {
template <>
struct hash<Vec2> {
  size_t operator()(const Vec2& vec) const {
    size_t seed{0};
    HashCombine(seed, vec.x, vec.y);
    return seed;
  }
};

}  // namespace std

#endif  // CORE_VEC2_H
