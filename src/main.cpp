#include <fmt/base.h>

#include "calculator/calculator.h"

int main() {
  fmt::print("Hello, World! {}\n", Calculator::Sum(1, 1));
  return 0;
}
