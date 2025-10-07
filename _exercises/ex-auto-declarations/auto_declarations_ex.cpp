#include <iostream>
#include <type_traits>
#include <vector>

using namespace std;

int x{42};
int y{665};
const int cx = 1;
int tab[10] = {};
vector<int> vec = {1, 2, 3};
const vector<int> cvec = {1, 2, 3};

using TODO = void;

void pointers()
{
    // pointer
    int* ptr = &x;
    *ptr = 888;
    ptr = &y;

    // pointer to const
    const int* ptr_to_const = &x;
    const int& ref_to_const = x;
    int const* ptr_to_const_alt = &x;
    int const& ref_to_const_alt = x;
    //*ptr_to_const = 333; // ERROR
    ptr_to_const = &y;

    // const pointer
    int* const const_ptr = &x;
    *const_ptr = 888;
    // const_ptr = &y; // ERROR

    // const pointer to const
    const int* const const_ptr_to_const = &x;
    int const* const const_ptr_to_const_alt = &y;
}

int main()
{
    auto a1 = x;
    static_assert(is_same<int, decltype(a1)>::value, "Error");

    auto a2 = &x;
    static_assert(is_same<int*, decltype(a2)>::value, "Error");

    const auto a3 = &x;
    static_assert(is_same<int* const, decltype(a3)>::value, "Error");

    auto* a4 = &x;
    static_assert(is_same<int*, decltype(a4)>::value, "Error");

    auto a5 = &cx;
    static_assert(is_same<const int*, decltype(a5)>::value, "Error");

    auto* a6 = &cx;
    static_assert(is_same<const int*, decltype(a6)>::value, "Error");

    int& rx = x;
    const int& crx = x;

    const auto& a7 = rx;
    static_assert(is_same<const int&, decltype(a7)>::value, "Error");

    auto a8 = crx;
    static_assert(is_same<int, decltype(a8)>::value, "Error");

    auto& a9 = crx;
    static_assert(is_same<const int&, decltype(a9)>::value, "Error");

    auto a10 = tab;
    static_assert(is_same<int*, decltype(a10)>::value, "Error");

    auto& a11 = tab;
    static_assert(is_same<int (&)[10], decltype(a11)>::value, "Error");

    auto a12 = vec.begin();
    static_assert(is_same<std::vector<int>::iterator, decltype(a12)>::value, "Error");

    auto a13 = vec.cbegin();
    static_assert(is_same<std::vector<int>::const_iterator, decltype(a13)>::value, "Error");

    auto a14 = vec.back();
    static_assert(is_same<int, decltype(a14)>::value, "Error");

    auto& a15 = cvec.back();
    static_assert(is_same<const int&, decltype(a15)>::value, "Error");

    const auto a16 = vec[0];
    static_assert(is_same<const int, decltype(a16)>::value, "Error");

    auto&& a17 = 7.5;
    static_assert(is_same<double&&, decltype(a17)>::value, "Error");

    auto&& a18 = crx;
    static_assert(is_same<const int&, decltype(a18)>::value, "Error");

    auto&& a19 = vec.front();
    static_assert(is_same<int&, decltype(a19)>::value, "Error");

    auto&& a20 = vec.begin();
    static_assert(is_same<std::vector<int>::iterator&&, decltype(a20)>::value, "Error");
}