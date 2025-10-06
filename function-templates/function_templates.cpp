#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

template <typename T>
T max_value(T a, T b)
{
    return a < b ? b : a;
}

// full template function specialization
// template <>
// const char* max_value(const char* cstr1, const char* cstr2)
// {
//     return std::strcmp(cstr1, cstr2) < 0 ? cstr2 : cstr1;
// }

const char* max_value(const char* cstr1, const char* cstr2);

const char* max_value(const char* cstr1, const char* cstr2)
{
    return std::strcmp(cstr1, cstr2) < 0 ? cstr2 : cstr1;
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

TEST_CASE("template function specialization")
{
    const char* txt1 = "Alaska";
    const char* txt2 = "Ala";

    REQUIRE(max_value<const char*>(txt1, txt2) == "Alaska"s);
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

    REQUIRE(is_greater(x, sx));        // full deduction
    REQUIRE(is_greater<short>(x, sx)); // partial deduction
}

TEST_CASE("address of function template")
{
    auto ptr_fun_1 = &max_value<int>;             // explicit params
    short (*ptr_fun_2)(short, short) = max_value; // now params are deduced
}

namespace ReturnTypes
{
    namespace ExplicitTResult
    {
        template <typename TResult, typename T1, typename T2>
        TResult max_value(T1 val1, T2 val2)
        {
            return val1 < val2 ? val2 : val1;
        }
    } // namespace ExplicitTResult

    namespace Auto
    {
        template <typename T1, typename T2>
        auto max_value(T1 val1, T2 val2) //-> decltype(val1 < val2 ? val2 : val1)
        {
            return val1 < val2 ? val2 : val1;
        }

        namespace Cpp20
        {
            auto max_value(auto val1, auto val2)
            {
                return val1 < val2 ? val2 : val1;
            }
        } // namespace Cpp20
    } // namespace Auto

    namespace Trait
    {
        template <typename T1, typename T2>
        std::common_type_t<T1, T2> max_value(T1 val1, T2 val2)
        {
            if (val1 < val2)
                return val2;
            return val1;
        }
    } // namespace Trait

    namespace DefaultParam
    {
        template <typename T1, typename T2, typename TResult = std::common_type_t<T1, T2>>
        TResult max_value(T1 val1, T2 val2)
        {
            if (val1 < val2)
                return val2;
            return val1;
        }
    } // namespace DefaultParam
} // namespace ReturnTypes

TEST_CASE("Choosing return types")
{
    SECTION("explicit template param")
    {
        auto result = ReturnTypes::ExplicitTResult::max_value<double>(42, 42.1);
    }

    SECTION("auto")
    {
        int x = 42;
        short sx = 22;

        auto result = ReturnTypes::Auto::max_value(x, sx);
    }

    SECTION("using trait")
    {
        SECTION("int & short -> int")
        {
            int x = 42;
            short sx = 22;
            auto result = ReturnTypes::Trait::max_value(x, sx);
        }

        SECTION("string & const char* -> string")
        {
            std::string txt1 = "Ala";
            const char* txt2 = "Alaska";

            auto result = ReturnTypes::Trait::max_value(txt1, txt2);
        }
    }

    SECTION("default template param")
    {
        int x = 42;
        short sx = 22;
        auto result = ReturnTypes::DefaultParam::max_value<int, short, short>(x, sx);
    }
}

///////////////////////////////////
// C++20 - auto + templates
auto multiply(auto a, auto b)
{
    decltype(a) target{};

    return a + b;
}

namespace IsIterpretedAs
{
    template <typename T1, typename T2>
    auto multiply(T1 a, T2 b)
    {
        T1 target{};
        return a + b;
    }
} // namespace IsIterpretedAs

//////////////////////////////////////
// passing function to function

template <typename TFunction>
void call(TFunction f, int arg)
{
    f(arg);
}

void foo(int arg)
{
    std::cout << std::format("foo({})\n", arg);
}

TEST_CASE("passing function to function")
{
    call(foo, 42);

    int local = 665;
    call([local](int x) { std::cout << std::format("lambda[{}]({})\n", local, x); }, 42);
}