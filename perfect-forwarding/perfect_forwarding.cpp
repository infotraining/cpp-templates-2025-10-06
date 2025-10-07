#include <catch2/catch_test_macros.hpp>
#include "gadget.hpp"

//#define MSVC

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif


////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

void have_fun(const Gadget& cg)
{
    puts(__PRETTY_FUNCTION__);
    cg.use();
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

// void use(Gadget& g)
// {
//     have_fun(g);
// }

// void use(const Gadget& g)
// {
//     have_fun(g);
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g));
// }

template <typename TGadget>
void use(TGadget&& g)
{
    have_fun(std::forward<TGadget>(g));

    // conceptually interpreted as
    // if constexpr(std::is_lvalue_reference_v<TGadget>)
    // {
    //     have_fun(g);
    // }
    // else
    // {
    //     have_fun(std::move(g));
    // }
}

namespace Cpp20
{
    void use(auto&& g)
    {
        have_fun(std::forward<decltype(g)>(g));
    }
}

std::string get_full_name(const std::string& fn, const std::string& ln)
{
    return fn + " " + ln;
}

TEST_CASE("get_full_name")
{
    std::string name = "Jan";
    std::string full_name = get_full_name(name, "Kowalski");
}

TEST_CASE("custom forwarding")
{
    Gadget g{1, "gadget"};
    const Gadget cg{2, "const-gadget"};

    use(g); 
    use(cg);
    use(Gadget{3, "temp-gadget"});
}

template <typename TTracer>
auto fun_with_tracer(TTracer&& tracer)
{
    auto inner_arg = std::forward<TTracer>(tracer);
    std::cout << "In fun_with_tracer: " << inner_arg << "\n";

    return inner_arg; // NRVO or move to caller
}

template <typename TArg>
auto use_lambda(TArg&& arg)
{
    auto lambda = [inner_arg = std::forward<TArg>(arg)] mutable { 
        std::cout << "From lambda: " << inner_arg << "\n";

        return std::move(inner_arg); // move to caller
    };

    std::cout << "From use_lambda: " << arg << "\n";

    return lambda();
}

struct Tracer
{
    Tracer()
    {
        std::cout << "Tracer::Tracer()\n";
    }

    Tracer(const Tracer&)
    {
        std::cout << "Tracer::Tracer(const Tracer&)\n";
    }

    Tracer(Tracer&&)
    {
        std::cout << "Tracer::Tracer(Tracer&&)\n";
    }

    ~Tracer()
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Tracer&)
    {
        return os << "Tracer instance";
    }
};

TEST_CASE("lambda + perfect forward")
{
    Tracer t;
    use_lambda(t);            
    std::cout << "----\n";
    auto t2 = use_lambda(Tracer{});     
    std::cout << "----\n";
    auto t3 = fun_with_tracer(Tracer{});       
}