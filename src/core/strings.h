#ifndef CORE_STRINGS_H
#define CORE_STRINGS_H

#include <string>
#include <vector>

class StringsUtils {
public:
  [[nodiscard]] static std::vector<std::string> Split(const std::string &str, char delimiter);
};

#endif  // CORE_STRINGS_H
