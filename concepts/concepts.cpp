#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

namespace ver_1
{
    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires std::is_pointer_v<T> // requires clause
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return *a < *b ? *b : *a;
    }

    //////////////////////////////////////////////////////

    void print(auto value)
    {
        std::cout << "value: " << value << "\n";
    }

    void print(auto value)
        requires std::is_pointer_v<decltype(value)> // trailing requires clause
    {
        assert(value != nullptr);
        std::cout << "value " << *value << "\n";
    }
} // namespace ver_1

namespace ver_2
{
    template <typename T>
    concept Pointer = std::is_pointer_v<T>;

    static_assert(Pointer<int*>);
    static_assert(not Pointer<int>);

    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T>
    // requires Pointer<T>
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return *a < *b ? *b : *a;
    }

    //////////////////////////////////////////////////////

    void print(auto value)
    {
        std::cout << "value: " << value << "\n";
    }

    void print(Pointer auto value)
    // requires Pointer<decltype(value)>
    {
        assert(value != nullptr);
        std::cout << "value " << *value << "\n";
    }
} // namespace ver_2

namespace ver_3
{
    template <typename T>
    concept Pointer = requires(T ptr) {
        *ptr;
        ptr == nullptr;
        ptr != nullptr;
    };

    static_assert(Pointer<int*>);
    static_assert(not Pointer<int>);

    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T>
    // requires Pointer<T>
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return *a < *b ? *b : *a;
    }

    //////////////////////////////////////////////////////

    void print(auto value)
    {
        std::cout << "value: " << value << "\n";
    }

    void print(Pointer auto value)
    // requires Pointer<decltype(value)>
    {
        assert(value != nullptr);
        std::cout << "value " << *value << "\n";
    }
} // namespace ver_3

TEST_CASE("concepts")
{
    using namespace ver_3;
    CHECK(max_value(42, 665) == 665);

    std::integral auto x = 42;
    int y = 665;
    CHECK(max_value(&x, &y) == 665);

    auto sp_1 = std::make_shared<int>(42);
    auto sp_2 = std::make_shared<int>(665);
    CHECK(max_value(sp_1, sp_2) == 665);
}

template <typename T>
concept Range = requires(T& rng) {
    std::begin(rng);
    std::end(rng);
};

static_assert(Range<std::vector<int>>);
static_assert(Range<std::string>);
static_assert(not Range<int>);
static_assert(Range<int[10]>);

template <Range T>
auto print(const T& container)
{
    for(const auto& item : container)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";
}