#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

int foo(int n)
{
    return n * 2;
}

template <int N>
struct Foo
{
    constexpr static int value = 2 * N;
};

template <int N>
inline constexpr int Foo_v = Foo<N>::value;

TEST_CASE("type traits")
{
    CHECK(foo(2) == 4);
    static_assert(Foo<2>::value == 4);
    static_assert(Foo_v<10> == 20);
}

template <typename T>
struct SizeOf
{
    constexpr static size_t value = sizeof(T);
};

template <typename T>
inline constexpr size_t SizeOf_v = SizeOf<T>::value;

TEST_CASE("SizeOf")
{
    static_assert(SizeOf_v<char> == 1);
}

// Identity

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

// IsSame

template <typename T, typename U>
struct IsSame
{
    constexpr static bool value = false;
};

template <typename T>
struct IsSame<T, T>
{
    constexpr static bool value = true;
};

template <typename T, typename U>
inline constexpr bool IsSame_v = IsSame<T, U>::value;

TEST_CASE("Identity")
{
    static_assert(IsSame_v<Identity_t<int>, int>);
    static_assert(IsSame_v<Identity_t<double>, double>);
    static_assert(IsSame_v<Identity_t<int>, double> == false);
}

template <typename T>
struct RemoveReference
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&>
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&&>
{
    using type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::type;

TEST_CASE("RemoveReference")
{
    static_assert(IsSame_v<RemoveReference_t<int>, int>);
    static_assert(IsSame_v<RemoveReference_t<int&>, int>);
    static_assert(IsSame_v<RemoveReference_t<int&&>, int>);
}
namespace Exercise
{
    template <typename InputIterator>
    auto accumulate(InputIterator first, InputIterator last)
    {
        //std::remove_const_t<RemoveReference_t<decltype(*first)>> sum{}; // int sum{};
        std::remove_cvref_t<decltype(*first)> sum{}; // int sum{};
        for (auto it = first; it != last; ++it)
        {
            sum += *it;
        }
        return sum;
    }
} // namespace Exercise

TEST_CASE("accumulate with traits")
{
    const std::vector<int> vec = {1, 2, 3};

    REQUIRE(Exercise::accumulate(vec.begin(), vec.end()) == 6);
}

template <typename T>
struct IsVoid
{
    static constexpr bool value = false;
};

template <>
struct IsVoid<void>
{
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool IsVoid_v = IsVoid<T>::value;

template <typename T>
void never_void(T* ptr)
{
    static_assert(!IsVoid_v<T>, "T cannot be void");
}

/////////////////////////
// IsPointer

template <typename T>
struct IsPointer : std::false_type
{
};

template <typename T>
struct IsPointer<T*> : std::true_type
{
};

template <typename T>
inline constexpr bool IsPointer_v = IsPointer<T>::value;

template <typename T>
void never_pointer(T value)
{
    static_assert(!IsPointer_v<T>, "T cannot be pointer type");
}

TEST_CASE("never void")
{
    int x = 42;
    //never_void<void>(&x);
    never_void<int>(&x);

    //never_pointer(&x);
    never_pointer(x);
}