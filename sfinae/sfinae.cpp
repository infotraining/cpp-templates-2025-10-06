#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std::literals;

auto is_power_of_2(int value)
{
    return value > 0 && (value & (value - 1)) == 0;
}

auto is_power_of_2(double value)
{
    int exponent;
    const double mantissa = std::frexp(value, &exponent);
    return mantissa == 0.5;
}

TEST_CASE("SFINAE")
{
    CHECK(true);
}