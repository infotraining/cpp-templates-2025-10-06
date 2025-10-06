#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

namespace TODO
{
    template <typename InputIterator, typename Predicate>
    InputIterator find_if(InputIterator begin, InputIterator end, Predicate pred)
    // requirements: bool pred(TValue value)
    {
        for (auto it = begin; it != end; ++it)
        {
            if (pred(*it))
            {
                return it;
            }
        }
        return end;
    }
} // namespace TODO

TEST_CASE("my find if")
{
    SECTION("happy path")
    {
        vector<int> vec = {1, 2, 3, 4, 665, 124, 4, 55, 234};

        auto is_lesser_evil = [](int x) {
            return x == 665;
        };

        auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);

        REQUIRE(pos != end(vec));
        REQUIRE(*pos == 665);
    }

    SECTION("sad path")
    {
        vector<int> vec = {1, 2, 3, 4, 124, 4, 55, 234};

        auto is_lesser_evil = [](int x) {
            return x == 665;
        };

        auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);

        REQUIRE(pos == end(vec));
    }
}

namespace TODO
{
    namespace ver_1
    {
        template <typename It, typename T = typename std::iterator_traits<It>::value_type>
        T accumulate(It begin, It end)
        {
            T result{};
            for (; begin != end; ++begin)
            {
                result += *begin;
            }
            return result;
        }
    } // namespace ver_1

    inline namespace ver_2
    {
        template <typename InputIterator>
        auto accumulate(InputIterator first, InputIterator last)
        {
            std::decay_t<decltype(*first)> sum{}; // int sum{};
            for (auto it = first; it != last; ++it)
            {
                sum += *it;
            }
            return sum;
        }
    } // namespace ver_2

    namespace ver_3
    {
        template <typename It>
        auto accumulate(It begin, It end)
        {
            auto sum = 0;
            for (It it = begin; it != end; ++it)
            {
                sum += *it;
            }

            return sum;
        }
    } // namespace ver_3

} // namespace TODO

namespace ExplainStd
{
    template <typename It, typename TZero>
    TZero accumulate(It begin, It end, TZero acc)
    {
        for (It it = begin; it != end; ++it)
        {
            acc += *it;
        }

        return acc;
    }

    template <typename TContainer>
    auto begin(TContainer& container)
    {
        return container.begin();
    }

    template <typename TContainer>
    auto end(TContainer& container)
    {
        return container.end();
    }

    template <typename TContainer>
    auto size(TContainer& container)
    {
        return container.size();
    }



    template<typename T, size_t N>
    T* begin(T (&arr)[N])
    {
        return arr;
    }

    template<typename T, size_t N>
    T* end(T (&arr)[N])
    {
        return arr + N;
    }

    template<typename T, size_t N>
    size_t size(T (&arr)[N])
    {
        return N;
    }
} // namespace ExplainStd

TEST_CASE("begin(container) & end(container)")
{
    SECTION("STL container")
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        REQUIRE(ExplainStd::begin(vec) == vec.begin());
        REQUIRE(ExplainStd::end(vec) == vec.end());
        REQUIRE(ExplainStd::size(vec) == 5);
    }

    SECTION("native arrays")
    {
        int tab[5] = {1, 2, 3, 4, 5};

        REQUIRE(ExplainStd::begin(tab) == &tab[0]);
        REQUIRE(ExplainStd::end(tab) == (tab + 5));
        REQUIRE(ExplainStd::size(tab) == 5);
    }
}

TEST_CASE("my accumulate")
{
    SECTION("ints")
    {
        const vector<int> data = {1, 2, 3, 4, 5};

        auto result = TODO::accumulate(begin(data), end(data));

        static_assert(std::is_same_v<decltype(result), int>);

        REQUIRE(result == 15);
    }

    SECTION("strings")
    {
        std::string words[] = {"one", "two", "three"};

        auto result = ExplainStd::accumulate(begin(words), end(words), "Tekst: "s);

        static_assert(std::is_same_v<decltype(result), std::string>);

        REQUIRE(result == "Tekst: onetwothree");
    }

    SECTION("double")
    {
        std::vector<double> vec = {1.2, 4.5, 43.22};

        double result = std::accumulate(vec.begin(), vec.end(), 0.0);
    }
}

namespace TODO
{
    // TODO: zero
}

// TEST_CASE("zero")
// {
//     using namespace TODO;

//     SECTION("vector of ints")
//     {
//         std::vector<int> numbers{1, 2, 3, 4};
//         zero(numbers);

//         REQUIRE(numbers == std::vector{0, 0, 0, 0});
//     }

//     SECTION("array of chars")
//     {
//         int buffer[10] = {1, 2, 3, 4};
//         zero(buffer);

//         REQUIRE(std::ranges::all_of(buffer, [](char b) { return b == 0; }));
//     }

//     SECTION("list of strings")
//     {
//         std::list<std::string> words = { "one", "two", "three" };

//         zero(words);

//         REQUIRE(words == std::list{""s, ""s, ""s});
//     }
// }
