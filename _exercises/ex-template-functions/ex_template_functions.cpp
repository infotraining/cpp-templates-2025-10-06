#include <algorithm>
#include <array>
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
    namespace Unconstrained
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
    } // namespace Unconstrained

    inline namespace Constrained
    {
        template <typename F, typename It>
        concept AlgorithmPredicate = std::predicate<F, typename std::iterator_traits<It>::value_type>;

        template <std::input_iterator It, AlgorithmPredicate<It> Predicate>
        It find_if(It begin, It end, Predicate pred)
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
    } // namespace Constrained
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

    template <typename T, size_t N>
    T* begin(T (&arr)[N])
    {
        return arr;
    }

    template <typename T, size_t N>
    T* end(T (&arr)[N])
    {
        return arr + N;
    }

    template <typename T, size_t N>
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
    template <typename TRange>
    struct Iterator
    {
        using type = decltype(std::begin(std::declval<TRange&>()));
    };

    template <typename TRange>
    using Iterator_t = typename Iterator<TRange>::type;

    template <typename TRange>
    struct RangeValue
    {
        using type = typename std::iterator_traits<Iterator_t<TRange>>::value_type;
    };

    template <typename TRange>
    using RangeValue_t = typename RangeValue<TRange>::type;

    static_assert(is_same_v<RangeValue_t<vector<int>>, int>);
    static_assert(is_same_v<RangeValue_t<list<string>>, string>);
    static_assert(is_same_v<RangeValue_t<array<double, 5>>, double>);
    static_assert(is_same_v<RangeValue_t<int[10]>, int>);

    template <typename TContainer>
    inline constexpr bool is_memset_friendly = std::contiguous_iterator<Iterator_t<TContainer>> && std::is_trivially_copyable_v<RangeValue_t<TContainer>>;

    enum class Implementation {
        Generic,
        Optimized
    };

    template <typename TContainer>
    Implementation zero(TContainer& container)
    {
        using T = RangeValue_t<TContainer>;

        if constexpr (is_memset_friendly<TContainer>)
        {
            std::memset(std::data(container), 0, sizeof(T) * std::size(container));
            return Implementation::Optimized;
        }
        else
        {
            for (auto&& item : container)
            {
                item = T{};
            }

            return Implementation::Generic;
        }

        // is interpreted as:
        // for(auto it = begin(container); it != end(container); ++it)
        // {
        //     auto&& item = *it;
        //     item = T{};
        // }
    }
} // namespace TODO

TEST_CASE("zero")
{
    using namespace TODO;

    SECTION("vector of ints")
    {
        std::vector<int> numbers{1, 2, 3, 4};
        REQUIRE(zero(numbers) == Implementation::Optimized);

        REQUIRE(numbers == std::vector{0, 0, 0, 0});
    }

    SECTION("vector of bool")
    {
        std::vector<bool> flags{true, false, false, true, true};
        REQUIRE(zero(flags) == Implementation::Generic);

        REQUIRE(flags == std::vector<bool>{false, false, false, false, false});
    }

    SECTION("array of chars")
    {
        uint8_t buffer[] = {1, 2, 3, 4};
        REQUIRE(zero(buffer) == Implementation::Optimized);

        REQUIRE(std::ranges::all_of(buffer, [](auto b) { return b == 0; }));
    }

    SECTION("list of strings")
    {
        std::list<std::string> words = {"one", "two", "three"};
        REQUIRE(zero(words) == Implementation::Generic);

        REQUIRE(words == std::list{""s, ""s, ""s});
    }
}
