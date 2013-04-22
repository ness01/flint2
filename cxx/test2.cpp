#include <iostream>
#include <cstdlib>

#include "generic.h"
#include <flint/fmpz.h>
#include <flint/fmpq.h>

#define DEBUG(expr) std::cout << expr << std::endl

// Definitions for fmpz
namespace flint {
namespace detail {
struct FMPZ // tag
{
    typedef fmpz_t underlying;

    static void empty_initialization(underlying& u)
    {
        DEBUG("fmpz_init");
        fmpz_init(u);
    }
    static void destruction(underlying& u)
    {
        DEBUG("fmpz_clear");
        fmpz_clear(u);
    }
    static void print(const underlying& u, std::ostream& o)
    {
        char* str = fmpz_get_str(0, 10, u);
        o << str;
        std::free(str);
    }
};
}

typedef wrapped_object<detail::FMPZ> Fmpz; // name collision otherwise?

namespace traits {
template<>
struct initialization<Fmpz, Fmpz>
{
    static void doit(fmpz_t& to, const fmpz_t& from)
    {
        DEBUG("fmpz_init_set");
        fmpz_init_set(to, from);
    }
};

template<>
struct assignment<Fmpz, Fmpz>
{
    static void doit(fmpz_t& to, const fmpz_t& from)
    {
        DEBUG("fmpz_set");
        fmpz_set(to, from);
    }
};

template<class T>
struct assignment<Fmpz, T, typename meta::enable_if<basic_traits<T>::is_signed_integer>::res>
{
    static void doit(fmpz_t& to, T t)
    {
        DEBUG("fmpz_set_si");
        fmpz_set_si(to, static_cast<long>(t));
    }
};

template<class T>
struct assignment<Fmpz, T, typename meta::enable_if<basic_traits<T>::is_unsigned_integer>::res>
{
    static void doit(fmpz_t& to, T t)
    {
        DEBUG("fmpz_set_ui");
        fmpz_set_si(to, static_cast<unsigned long>(t));
    }
};
} // traits
} // flint

// Definitions for fmpq
namespace flint {
namespace detail {
struct FMPQ // tag
{
    typedef fmpq_t underlying;

    static void empty_initialization(underlying& u)
    {
        DEBUG("fmpq_init");
        fmpq_init(u);
    }
    static void destruction(underlying& u)
    {
        DEBUG("fmpq_clear");
        fmpq_clear(u);
    }
    static void print(const underlying& u, std::ostream& o)
    {
        char* str = fmpq_get_str(0, 10, u);
        o << str;
        std::free(str);
    }
};
}

typedef wrapped_object<detail::FMPQ> Fmpq; // name collision otherwise?

namespace traits {
template<>
struct assignment<Fmpq, Fmpq>
{
    static void doit(fmpq_t& to, const fmpq_t& from)
    {
        DEBUG("fmpq_set");
        fmpq_set(to, from);
    }
};

template<class T>
struct assignment<Fmpq, T, typename meta::enable_if<basic_traits<T>::is_signed_integer>::res>
{
    static void doit(fmpq_t& to, T t)
    {
        DEBUG("fmpq_set_si");
        fmpq_set_si(to, static_cast<long>(t), 1);
    }
};
} // traits
} // flint

// fmpz and fmpq interaction
namespace flint {
static const Fmpz one(1);
namespace traits {
template<>
struct assignment<Fmpq, Fmpz>
{
    static void doit(fmpq_t& to, const fmpz_t& from)
    {
        DEBUG("fmpq_set_fmpz_frac");
        fmpq_set_fmpz_frac(to, from, one.get_data());
    }
};
}
}

int
main()
{
    using namespace flint;

    Fmpz a(-4);
    std::cout << a << std::endl;
    Fmpz b(a);
    std::cout << b << std::endl;
    Fmpz c(5u);
    std::cout << c << std::endl;

    // this causes a compile-time error:
    //Fmpz d("foo");

    Fmpq q(17);
    std::cout << q << std::endl;
    Fmpq p(a);
    std::cout << p << std::endl;
}
