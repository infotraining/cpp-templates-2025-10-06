#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce2(T& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce3(T&& arg) // universal reference
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}


TEST_CASE("type deduction rules")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = x;
    int tab[10];

    SECTION("Case 1")
    {
        deduce1(x);     // T: int
        deduce1(cx);    // T: int  
        deduce1(ref_x); // T: int 
        deduce1(cref_x);// T: int 
        deduce1(tab);   // T: int* - array decays to pointer 
        deduce1(foo);   // T: void(*)(int) - function decays to pointer 

        auto a1 = x;      // int a1 = x;
        auto a2 = cx;     // int a2 = cx;
        auto a3 = ref_x;  // int a3 = ref_x;
        auto a4 = cref_x; // int a4 = cref_x;
        auto a5 = tab;    // int* a5 = tab;
        auto a6 = foo;    // void(*a6)(int) = foo;
    }

    SECTION("Case 2")
    {
        deduce2(x);       // deduce2<int>(int& arg)     
        deduce2(cx);      // deduce2<const int>(const int& arg)    
        deduce2(ref_x);   // deduce2<int>(int& arg)  
        deduce2(cref_x);  // deduce2<const int>(const int&)  
        deduce2(tab);     // deduce2<int[10]>(int(&arg)[10])
        deduce2(foo);     // deduce2<void(int)>(void(&arg)(int)) 
        //deduce2(std::string("text"));

        auto& a1 = x;     // int& a1 = x;
        auto& a2 = cx;    // const int& a2 = cx; 
        auto& a3 = ref_x; // int& a3 = ref_x;
        auto& a4 = cref_x;// const int& a4 = cref_x;
        auto& a5 = tab;   // int(&a5)[10] = tab;
        auto& a6 = foo;   // void(&a6)(int) = foo;
    }

    // reference collapsing
    // & &   -> & // before C++11
    // && &  -> &
    // & &&  -> &
    // && && -> &&

    SECTION("Case 3")
    {
        deduce3(x);      // deduce3<int&>(int& && -> int& arg)           
        deduce3(cx);     // deduce3<const int&>(const int& && -> const int&)          
        deduce3(ref_x);  // deduce3<int&>(int& && -> int&)        
        deduce3(cref_x); // deduce3<const int&>(const int& && -> const int&)         
        deduce3(tab);    // deduce3<int(&)[10]>(int(&)[10] && -> int(&)[10])        
        deduce3(foo);    // deduce3<void(&)(int)>(void(&)(int) && -> void(&)(int))        
        deduce3(string("text")); // deduce3<std::string>(std::string&& arg) 

        // universal reference
        auto&& a1 = x;      // int& a1 = x;
        auto&& a2 = cx;     // const int& a2 = cx;
        auto&& a3 = ref_x;  // int& a3 = ref_x;
        auto&& a4 = cref_x; // const int& a4 = cref_x;
        auto&& a5 = tab;    // int(&a5)[10] = tab;
        auto&& a6 = foo;    // void(&a6)(int) = foo;
        auto&& a7 = string("text"); // std::string&& z7 = "text"s;

        std::string&& a8 = std::string("text");
    }
}

/////////////////////////////
// passing by ref

template <typename T>
auto process(T&& arg)
{
    arg += "!!!";
    return arg;
}

std::string get_text()
{
    return "text";
}

TEST_CASE("passing by ref")
{
    std::string str = "text"; 
    process(str);
    REQUIRE(str == "text!!!");

    std::string result = process(get_text());
    REQUIRE("text!!!");
}



TEST_CASE("decltype(auto)")
{

}