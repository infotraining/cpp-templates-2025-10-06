#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <list>

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

template <std::convertible_to<bool> Flag> // std::convertible_to<Flag, bool>
void fun_with_flags(Flag flag)
{}

template <typename T>
concept Indexable = requires(T obj, size_t index)
{
    obj[index];
};

template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } noexcept -> std::convertible_to<T>;
};

template <typename T>
concept Fooable = requires(const T& obj) {
    obj.bar();
};

struct Foo
{
    void bar() const
    {}
};

static_assert(Fooable<Foo>);

struct Value
{
    int v;

    Value operator+(const Value& other) noexcept
    {
        return Value{v + other.v};
    }
};

static_assert(Addable<Value>);

// template <typename T>
// concept BigType = requires {
//     requires sizeof(T) >= 16;
// };

template <typename T>
concept BigType = (sizeof(T) >= 16);

static_assert(BigType<std::vector<int>>);
static_assert(not BigType<char>);

TEST_CASE("convertible_to")
{
    fun_with_flags(42);
    //fun_with_flags(std::vector{1, 2, 3});
}

//////////////////////////////////////
// Subsumig concepts

struct BoundingBox
{
    int w, h;
};

struct Color
{
    int r, g, b;
};

template <typename T>
concept Shape = requires(T obj) {
    { obj.box() } -> std::same_as<BoundingBox>;
    obj.draw();
};

template <typename T>
concept ShapeWithColor = Shape<T> && requires(T obj, Color c) {
    obj.set_color(c);
    { obj.get_color() } -> std::convertible_to<Color>;
};

template <typename T>
concept ShapeWithShading = ShapeWithColor<T> && requires(T obj, Color c) {
    obj.shade();
};

struct Rect
{
    int width, height;
    Color color{};

    BoundingBox box() const
    {
        return {width, height};
    }

    void draw() const
    {
        std::cout << "Drawing Rect: " << width << ", " << height << "\n";
    }

    void set_color(Color new_color)
    {
        std::cout << "Setting color...\n";
        color = new_color;
    }

    Color get_color() const
    {
        return color;
    }

    // void shade() 
    // {
    //     std::cout << "Shading rect...\n";
    // }
};

static_assert(Shape<Rect>);
static_assert(ShapeWithColor<Rect>);
static_assert(not ShapeWithShading<Rect>);


void render(Shape auto shp)
{
    shp.draw();
}

void render(ShapeWithColor auto shp)
{
    shp.set_color(Color{0, 0, 0});
    shp.draw();
}

void render(ShapeWithShading auto shp)
{
    shp.set_color(Color{0, 0, 0});
    shp.shade();
    shp.draw();
}

TEST_CASE("subsuming concepts")
{
    Rect rect{10, 20};

    render(rect);
}

void my_sort(std::ranges::random_access_range auto container)
{
    std::sort(container.begin(), container.end());
}

void my_sort(std::ranges::bidirectional_range auto container)
{
    container.sort();
}

TEST_CASE("sorting & subsuming")
{
    std::vector vec = {42, 1, 665};
    my_sort(vec);

    std::list lst = {42, 1, 665};
    my_sort(lst);
}