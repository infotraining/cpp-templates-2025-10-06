#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

template <typename T>
T max_value(T a, T b)
{
    return a < b ? b : a;
}

struct Person
{
    std::string name;
    uint8_t age;

    auto operator<=>(const Person&) const = default;
};

TEST_CASE("function templates")
{
    SECTION("int")
    {
        int x = 42;
        int y = 665;
        REQUIRE(max_value(x, y) == 665);
    }

    SECTION("double")
    {
        double dx = 3.14;
        double dy = 2.71;

        REQUIRE(max_value(dx, dy) == 3.14);
    }

    SECTION("std::string")
    {
        std::string str1 = "Ala";
        std::string str2 = "Alaska";

        REQUIRE(max_value(str1, str2) == "Alaska");
    }

    SECTION("Person")
    {
        Person p1{"Jan", 42};
        Person p2{"Jan", 43};

        Person gtp = max_value(p1, p2);

        REQUIRE(gtp == Person{"Jan", 43});
    }
}

TEST_CASE("resolving deduction conflicts")
{
    int x = 42;
    short sx = 23;

    SECTION("static_cast")
    {
        REQUIRE(max_value(x, static_cast<int>(sx)) == 42);
    }

    SECTION("disabling deduction")
    {
        REQUIRE(max_value<int>(x, sx) == 42);
    }
}

template <typename T, typename U>
bool is_greater(const T& val1, const U& val2)
{
    return val1 > val2;
}

TEST_CASE("partial deduction")
{
    int x = 42;
    short sx = 23;

    REQUIRE(is_greater(x, sx)); // full deduction
    REQUIRE(is_greater<short>(x, sx)); // partial deduction
} 