#include <catch2/catch_test_macros.hpp>

#include "calculator/calculator.h"

int Factorial(int number) {
  return number <= 1 ? 1 : Factorial(number - 1) * number;  // pass
}

TEST_CASE("Factorial", "[factorial]") {
  REQUIRE(Factorial(5) == 120);
  REQUIRE_FALSE(Factorial(5) == 1);
}

// cppcheck-suppress knownConditionTrueFalse
TEST_CASE("Calculator", "[calculator]") { REQUIRE(Calculator::Sum(1, 1) == 2); }
