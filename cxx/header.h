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
struct plus {};
struct times {};

template<class Left, class Op, class Right>
struct binary_expr
{
    const Left& left;
    const Right& right;

    binary_expr(const Left& l, const Right& r)
        : left(l), right(r)
    {
    }

    template<class R>
    binary_expr<binary_expr<Left, Op, Right>, plus, R> operator+(const R& r)
    {
        return binary_expr<binary_expr<Left, Op, Right>, plus, R>(*this, r);
    }

    template<class R>
    binary_expr<binary_expr<Left, Op, Right>, times, R> operator*(const R& r)
    {
        return binary_expr<binary_expr<Left, Op, Right>, times, R>(*this, r);
    }
};

class fmpz
{
private:
    fmpz_t data;

public:
    fmpz()
    {
        DEBUG("fmpz() [fmpz_init]");
        fmpz_init(data);
    }
    fmpz(long i)
    {
        DEBUG("fmpz(" << i << ") [fmpz_init; fmpz_set_si]");
        fmpz_init(data);
        fmpz_set_si(data, i);
    }
    fmpz(const fmpz& o)
    {
        DEBUG("fmpz copy [fmpz_init_set]");
        fmpz_init_set(data, o.data);
        //fmpz_init(data);
        //fmpz_set(data, o.data);
    }
#ifdef MOVE_SEMANTICS
    fmpz(fmpz&& o)
    {
        DEBUG("fmpz move [fmpz_init; fmpz_swap]");
        fmpz_init(data);
        fmpz_swap(data, o.data);
    }
#endif

    ~fmpz()
    {
        DEBUG("~fmpz() [fmpz_clear]");
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

#ifdef EXPRESSION_TEMPLATES
    template<class Right>
    binary_expr<fmpz, plus, Right>
    operator+(const Right& right)
    {
        return binary_expr<fmpz, plus, Right>(*this, right);
    }

    template<class Right>
    binary_expr<fmpz, times, Right>
    operator*(const Right& right)
    {
        return binary_expr<fmpz, times, Right>(*this, right);
    }

    template<class Left, class Op, class Right>
    fmpz(const binary_expr<Left, Op, Right>& expr);
#endif
};


#ifdef EXPRESSION_TEMPLATES
template<unsigned N>
struct temporaries
{
    fmpz head;
    temporaries<N-1> tail;
};
template<>
struct temporaries<0>
{
};


template<class T>
struct evaluator;
// contract:
//{
//    static const unsigned ntemporaries;
//
//    // may only be instantiated with N >= ntemporaries
//    template<unsigned N>
//    static void eval(fmpz& result, temporaries<N>& temps, const T& data);
//}

template<bool expr, class True, class False>
struct if_;

template<class True, class False>
struct if_<true, True, False>
{
    typedef True result;
};

template<class True, class False>
struct if_<false, True, False>
{
    typedef False result;
};

template<class Op, class Evalop>
struct evalhelper
{
    template<class Left, class Right>
    struct eval_generic
    {
        typedef evaluator<Left> lefteval_t;
        typedef evaluator<Right> righteval_t;
        typedef binary_expr<Left, Op, Right> T;
        static const unsigned ltemps = lefteval_t::ntemporaries;
        static const unsigned rtemps = righteval_t::ntemporaries;
        static const unsigned ntemporaries = (ltemps == rtemps) ?
            ltemps + 1 : (ltemps > rtemps ? ltemps : rtemps);

        struct internal_helper_true
        {
            template<unsigned N>
            static void eval(fmpz& result, temporaries<N>& temps, const T& data)
            {
                lefteval_t::eval(result, temps, data.left);
                righteval_t::eval(temps.head, temps.tail, data.right);
            }
        };
        struct internal_helper_false
        {
            template<unsigned N>
            static void eval(fmpz& result, temporaries<N>& temps, const T& data)
            {
                righteval_t::eval(result, temps, data.right);
                lefteval_t::eval(temps.head, temps.tail, data.left);
            }
        };

        template<unsigned N>
        static void eval(fmpz& result, temporaries<N>& temps, const T& data)
        {
            if_<
                (ltemps > rtemps),
                internal_helper_true,
                internal_helper_false
               >::result::eval(result, temps, data);
            Evalop::eval(result, result, temps.head);
        }
    };

    struct eval_2
    {
        static const unsigned ntemporaries = 0;
        typedef binary_expr<fmpz, Op, fmpz> T;

        template<unsigned N>
        static void eval(fmpz& result, temporaries<N>& temps, const T& data)
        {
            Evalop::eval(result, data.left, data.right);
        }
    };

    template<class Right>
    struct eval_r
    {
        typedef evaluator<Right> eval_t;
        static const unsigned ntemporaries = eval_t::ntemporaries;
        typedef binary_expr<fmpz, Op, Right> T;

        template<unsigned N>
        static void eval(fmpz& result, temporaries<N> temps, const T& data)
        {
            eval_t::eval(result, temps, data.right);
            Evalop::eval(result, data.left, result);
        }
    };

    template<class Left>
    struct eval_l
    {
        typedef evaluator<Left> eval_t;
        static const unsigned ntemporaries = eval_t::ntemporaries;
        typedef binary_expr<Left, Op, fmpz> T;

        template<unsigned N>
        static void eval(fmpz& result, temporaries<N> temps, const T& data)
        {
            eval_t::eval(result, temps, data.left);
            Evalop::eval(result, result, data.right);
        }
    };
};

struct pluseval
{
    static void eval(fmpz& result, const fmpz& l, const fmpz& r)
    {
        result.add(l, r);
    }
};

template<class Left, class Right>
struct evaluator<binary_expr<Left, plus, Right> >
    : public evalhelper<plus, pluseval>::eval_generic<Left, Right>
{ };

template<class Left>
struct evaluator<binary_expr<Left, plus, fmpz> >
    : public evalhelper<plus, pluseval>::eval_l<Left>
{
};

template<class Right>
struct evaluator<binary_expr<fmpz, plus, Right> >
    : public evalhelper<plus, pluseval>::eval_r<Right>
{
};

template<>
struct evaluator<binary_expr<fmpz, plus, fmpz> >
    : public evalhelper<plus, pluseval>::eval_2
{ };

struct timeseval
{
    static void eval(fmpz& result, const fmpz& l, const fmpz& r)
    {
        result.mul(l, r);
    }
};

template<class Left, class Right>
struct evaluator<binary_expr<Left, times, Right> >
    : public evalhelper<times, timeseval>::eval_generic<Left, Right>
{ };

template<class Left>
struct evaluator<binary_expr<Left, times, fmpz> >
    : public evalhelper<times, timeseval>::eval_l<Left>
{
};

template<class Right>
struct evaluator<binary_expr<fmpz, times, Right> >
    : public evalhelper<times, timeseval>::eval_r<Right>
{
};

template<>
struct evaluator<binary_expr<fmpz, times, fmpz> >
    : public evalhelper<times, timeseval>::eval_2
{ };


template<class Left, class Op, class Right>
inline fmpz::fmpz(const binary_expr<Left, Op, Right>& expr)
{
    DEBUG("expression template instantiation [fmpz_init]");
    fmpz_init(data);
    typedef evaluator<binary_expr<Left, Op, Right> > eval_t;
    temporaries<eval_t::ntemporaries> temps;
    eval_t::eval(*this, temps, expr);
}
#endif
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
