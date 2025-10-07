#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

template <typename T, size_t N>
struct Array
{
    T items[N];

    using iterator = T*;
    using const_iterator = const T*;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    size_t size() const;

    iterator begin()
    {
        return items;
    }

    iterator end()
    {
        return items + N;
    }

    const_iterator begin() const
    {
        return items;
    }

    const_iterator end() const
    {
        return items + N;
    }

    // reference operator[](size_t n)
    // {
    //     return items[n];
    // }

    // const_reference operator[](size_t n) const
    // {
    //     return this->items[n];
    // }

    template <typename TSelf>
    auto& operator[](this TSelf&& self, size_t n);

    auto operator<=>(const Array& other) const = default;
};

template <typename T, size_t N>
size_t Array<T, N>::size() const
{
    return N;
}

template <typename T, size_t N>
    template <typename TSelf>
auto& Array<T, N>::operator[](this TSelf&& self, size_t n)
{
    return std::forward<TSelf>(self).items[n];
}

using namespace std::literals;

TEST_CASE("class templates")
{
    Array<int, 5> arr1 = {1, 2, 3, 4, 5};

    for(int item : arr1)
        std::cout << item << " ";
    std::cout << "\n";

    for(int& item : arr1)
        item *= 2;

    arr1[2] = 665;

    REQUIRE(arr1 == Array<int, 5>{2, 4, 6, 8, 10});
} 