#include "strings.h"

#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> StringsUtils::Split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss{str};

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}
