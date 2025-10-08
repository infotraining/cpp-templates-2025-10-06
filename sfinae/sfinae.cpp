#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

namespace HistoricMomentsOfCpp
{
    template <uintmax_t N>
    struct Factorial
    {
        constexpr static uintmax_t value = N * Factorial<N - 1>::value;
    };

    template <>
    struct Factorial<1>
    {
        constexpr static uintmax_t value = 1;
    };

    static_assert(Factorial<5>::value);
} // namespace HistoricMomentsOfCpp

template <bool Condition, typename T = void>
struct EnableIf
{
};

template <typename T>
struct EnableIf<true, T>
{
    using type = T;
};

template <bool Condition, typename T = void>
using EnableIf_t = typename EnableIf<Condition, T>::type;

namespace SfinaeIntro
{
    void sfinae(double d)
    {
        std::cout << "SFINAE(double: " << d << ")\n";
    }

    template <typename T, typename = EnableIf_t<std::is_integral_v<T>>>
    void sfinae(T x)
    {
        std::cout << "SFINAE<T>(" << x << ") enabled\n";
    }
} // namespace SfinaeIntro

TEST_CASE("SFINAE introduction")
{
    using namespace SfinaeIntro;

    sfinae(42.0f);
}

template <typename T>
auto is_power_of_2(T value) -> EnableIf_t<std::is_integral_v<T>, bool>
{
    return value > 0 && (value & (value - 1)) == 0;
}

template <typename T>
auto is_power_of_2(T value) -> EnableIf_t<std::is_floating_point_v<T>, bool>
{
    int exponent;
    const T mantissa = std::frexp(value, &exponent);
    return mantissa == 0.5;
}

template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
struct Data
{
    T value;
};

constexpr int check_sanity(int* evil_ptr)
{
    int* ptr = new int[1024];

    // ptr[105] = *evil_ptr;

    delete[] ptr;

    return 0;
}

constexpr int* evil_ptr{};
constexpr int test_result = check_sanity(evil_ptr);

void use_ptr(nullptr_t) = delete;

TEST_CASE("SFINAE")
{
    CHECK(is_power_of_2(8));
    CHECK(is_power_of_2(64));
    CHECK(is_power_of_2(256ULL));
    CHECK(!is_power_of_2(99L));

    CHECK(is_power_of_2(8.0));
    CHECK(is_power_of_2(64.0));
    CHECK(is_power_of_2(256.0));
    CHECK(!is_power_of_2(256.8));

    CHECK(is_power_of_2(8.0f));
    CHECK(is_power_of_2(64.0f));

    Data<double> d1{42.44};
    // Data<int> d2{665};

    // int* ptr = nullptr;
    // use_ptr(ptr);
}

namespace Cpp17
{
    template <typename T>
    bool is_power_of_2(T value)
    {
        if constexpr(std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == 0.5;
        }
    }

    template <size_t N>
    auto create_buffer()
    {
        if constexpr(N < 128)
        {
            return std::array<uint8_t, N>{};
        }
        else
        {
            return std::vector<uint8_t>(N);
        }
    }
} // namespace Cpp17

TEST_CASE("is_power_of to with if constexpr")
{
    CHECK(Cpp17::is_power_of_2(8));
    CHECK(Cpp17::is_power_of_2(64));
    CHECK(Cpp17::is_power_of_2(256ULL));
    CHECK(!Cpp17::is_power_of_2(99L));

    CHECK(Cpp17::is_power_of_2(8.0));
    CHECK(Cpp17::is_power_of_2(64.0));
    CHECK(Cpp17::is_power_of_2(256.0));
    CHECK(!Cpp17::is_power_of_2(256.8));

    CHECK(Cpp17::is_power_of_2(8.0f));
    CHECK(Cpp17::is_power_of_2(64.0f));

    auto small_buffer = Cpp17::create_buffer<64>();
    static_assert(std::is_same_v<decltype(small_buffer), std::array<uint8_t, 64>>);
    
    auto large_buffer = Cpp17::create_buffer<1024>();
    static_assert(std::is_same_v<decltype(large_buffer), std::vector<uint8_t>>);
}

template <typename, typename = void>
constexpr bool IsIterable{}; // false

template <typename T>
constexpr bool IsIterable<T, 
    std::void_t<
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end())>> = true;


template <typename T>
auto print(const T& container) -> std::enable_if_t<IsIterable<T>>
{
    for(const auto& item : container)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";
}

struct EvilContainer
{
    int data[3];
};


TEST_CASE("checking interface")
{
    std::vector<int> vec = {1, 2, 3};
    print(vec);

    EvilContainer ec{1, 2, 3};
    print(ec);
}