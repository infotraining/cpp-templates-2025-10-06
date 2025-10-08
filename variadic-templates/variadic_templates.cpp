#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

namespace ExplainStd
{
    template <typename T, typename... TArg>
    std::unique_ptr<T> make_unique(TArg&&... arg)
    {
        return std::unique_ptr<T>(new T(std::forward<TArg>(arg)...));
    }

    // template <typename T, typename TArg1, typename TArg2>
    // std::unique_ptr<T> make_unique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return std::unique_ptr<T>(new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
    // }
} // namespace ExplainStd

struct Gadget
{
    int id;
    std::string name;

    Gadget(int id)
        : id{id}
        , name{"noname"}
    { }

    Gadget(int id, std::string name)
        : id{id}
        , name{std::move(name)}
    { }
};

TEST_CASE("variadic templates - make_unique")
{
    auto ptr_g1 = ExplainStd::make_unique<Gadget>(42);
    auto ptr_g2 = ExplainStd::make_unique<Gadget>(42, "ipad");
}

// void print()
// {
//     std::cout << "\n";
// }

template <typename THead, typename... TTail>
void print(THead head, TTail... tail)
{
    std::cout << head << " ";
    
    if constexpr (sizeof...(tail) > 0)
        print(tail...);
    else
        std::cout << "\n";
}

TEST_CASE("print")
{
    print(1, 42.3, "ctext", "text"s);
}

template <typename... TArgs>
auto sum(TArgs... args)
{
    return (... + args);  // (((1 + 2) + 3) + 4) - fold expression
}

TEST_CASE("sum with fold expression")
{
    REQUIRE(sum(1, 2, 3, 4) == 10);
    REQUIRE(sum(1, 2, 3, 4, 5) == 15);
}

template <typename... Args>
bool all_true(Args... args)
{
    return (... && args);
}

TEST_CASE("all_true")
{
    REQUIRE(all_true(true, true, 1, 1));
    REQUIRE(all_true(true, false, 1, 1) == false);
}

template <typename... TArgs>
void print_fold(TArgs... args)
{
   (..., (std::cout << args << " ")); 
   std::cout << "\n";
}

TEST_CASE("print_fold")
{
    print_fold(1, 42, "ctext", "text"s);
}