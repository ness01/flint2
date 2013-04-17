#ifndef HEADER_H
#define HEADER_H HEADER_H

#include <flint/fmpz.h>

#include <cstdlib>

#ifdef INSTRUMENT
#define DEBUG(expr) std::cout << expr << std::endl
#else
#define DEBUG(expr)
#endif

namespace flint {
class fmpz
{
private:
    fmpz_t data;

public:
    fmpz()
    {
        DEBUG("fmpz()");
        fmpz_init(data);
    }
    fmpz(long i)
    {
        DEBUG("fmpz(" << i << ")");
        fmpz_init(data);
        fmpz_set_si(data, i);
    }
    fmpz(const fmpz& o)
    {
        DEBUG("fmpz copy");
        fmpz_init_set(data, o.data);
        //fmpz_init(data);
        //fmpz_set(data, o.data);
    }
#ifdef MOVE_SEMANTICS
    fmpz(fmpz&& o)
    {
        DEBUG("fmpz move");
        fmpz_init(data);
        fmpz_swap(data, o.data);
    }
#endif

    ~fmpz()
    {
        DEBUG("~fmpz()");
        fmpz_clear(data);
    }


    void print(std::ostream& o) const
    {
        char* str = fmpz_get_str(0, 10, data);
        o << str;
        std::free(str);
    }


    void add(const fmpz& l, const fmpz& r)
    {
        DEBUG("fmpz_add");
        fmpz_add(data, l.data, r.data);
    }
    void mul(const fmpz& l, const fmpz& r)
    {
        DEBUG("fmpz_mul");
        fmpz_mul(data, l.data, r.data);
    }

};
}

std::ostream& operator<<(std::ostream& o, const flint::fmpz& n)
{
    n.print(o);
    return o;
}

#ifndef EXPRESSION_TEMPLATES
flint::fmpz operator+ (const flint::fmpz& l, const flint::fmpz& r)
{
    flint::fmpz res;
    res.add(l, r);
    return res;
}

#ifdef MOVE_SEMANTICS
flint::fmpz operator+ (const flint::fmpz& l, flint::fmpz&& r)
{
    r.add(l, r);
    return std::move(r);
}
flint::fmpz operator+ (flint::fmpz&& l, const flint::fmpz&& r)
{
    l.add(l, r);
    return std::move(l);
}
flint::fmpz operator+ (flint::fmpz&& l, flint::fmpz&& r)
{
    r.add(l, r);
    return std::move(r);
}
#endif

flint::fmpz operator* (const flint::fmpz& l, const flint::fmpz& r)
{
    flint::fmpz res;
    res.mul(l, r);
    return res;
}

#ifdef MOVE_SEMANTICS
flint::fmpz operator* (const flint::fmpz& l, flint::fmpz&& r)
{
    r.mul(l, r);
    return std::move(r);
}
flint::fmpz operator* (flint::fmpz&& l, const flint::fmpz&& r)
{
    l.mul(l, r);
    return std::move(l);
}
flint::fmpz operator* (flint::fmpz&& l, flint::fmpz&& r)
{
    r.mul(l, r);
    return std::move(r);
}
#endif
#endif

#endif
