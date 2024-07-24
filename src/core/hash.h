#ifndef CORE_HASH_H
#define CORE_HASH_H

#include <functional>

// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x/57595105#57595105
template <typename T, typename... Rest>
void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);  // NOLINT
  (HashCombine(seed, rest), ...);
}

#endif  // CORE_HASH_H
