/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2013 Tom Bachmann

******************************************************************************/

#ifndef CXX_FMPZXX_H
#define CXX_FMPZXX_H

#include <cstdlib>

// TODO
// * non-explicit ctos
// * contexts

#include "cxx/evaluation_tools.h"
#include "cxx/expression.h"
#include "cxx/expression_traits.h"
#include "cxx/stdmath.h"

#include "fmpz.h"

namespace flint {
// "concrete" expression classes

namespace detail {
template<class T, class U> 
struct fmpzxx_enable_implicit : mp::false_ { };
template<class T> struct fmpzxx_traits;
} // detail

template<class Operation, class Data>
class fmpzxx_expression
    : public expression<derived_wrapper<fmpzxx_expression>, Operation, Data>
{
public:
    fmpzxx_expression() {}
    template<class T>
    explicit fmpzxx_expression(const T& t,
            typename mp::disable_if<detail::fmpzxx_enable_implicit<fmpzxx_expression, T> >::type* = 0)
        : expression<derived_wrapper< ::flint::fmpzxx_expression>,
              Operation, Data>(t) {}

    template<class T>
    fmpzxx_expression(const T& t,
            typename mp::enable_if<detail::fmpzxx_enable_implicit<fmpzxx_expression, T> >::type* = 0)
        : expression<derived_wrapper< ::flint::fmpzxx_expression>,
              Operation, Data>(t) {}

    template<class T>
    fmpzxx_expression(T& t,
            typename mp::enable_if<detail::fmpzxx_enable_implicit<fmpzxx_expression, T> >::type* = 0)
        : expression<derived_wrapper< ::flint::fmpzxx_expression>,
              Operation, Data>(t) {}

    template<class T>
    fmpzxx_expression& operator=(const T& t)
    {
        this->set(t);
        return *this;
    }

    typename detail::fmpzxx_traits<fmpzxx_expression>::nonconst_rt
        _fmpz() {return this->_data().f;}

    typename detail::fmpzxx_traits<fmpzxx_expression>::const_rt
    _fmpz() const {return this->_data().f;}

protected:
    explicit fmpzxx_expression(const Data& d)
        : expression<derived_wrapper< ::flint::fmpzxx_expression>,
              Operation, Data>(d) {}

    template<class D, class O, class Da>
    friend class expression;
};

namespace detail {
struct fmpz_data;
struct fmpzref_data;
struct fmpzcref_data;
}

typedef fmpzxx_expression<operations::immediate, detail::fmpz_data> fmpzxx;
typedef fmpzxx_expression<operations::immediate, detail::fmpzref_data> fmpzxx_ref;
typedef fmpzxx_expression<operations::immediate, detail::fmpzcref_data> fmpzxx_cref;

namespace traits {
template<> struct basetype<fmpzxx_ref> {typedef fmpzxx type;};
template<> struct basetype<fmpzxx_cref> {typedef fmpzxx type;};
template<> struct forwarding<fmpzxx> {typedef fmpzxx_cref type;};
template<> struct reference<fmpzxx> {typedef fmpzxx_ref type;};
template<> struct make_const<fmpzxx_ref> {typedef fmpzxx_cref type;};
}

namespace detail {
template<> struct fmpzxx_enable_implicit<fmpzxx_ref, fmpzxx> : mp::true_ { };
template<> struct fmpzxx_enable_implicit<fmpzxx_cref, fmpzxx> : mp::true_ { };
template<> struct fmpzxx_enable_implicit<fmpzxx_cref, fmpzxx_ref> : mp::true_ { };
// <fmpzxx, fmpzxx_cref> ?

template<class T> struct fmpzxx_traits
{
    typedef fmpz_t& nonconst_rt;
    typedef const fmpz_t& const_rt;
};
template<> struct fmpzxx_traits<fmpzxx_cref>
{
    typedef const fmpz* nonconst_rt;
    typedef const fmpz* const_rt;
};
template<> struct fmpzxx_traits<fmpzxx_ref>
{
    typedef fmpz* nonconst_rt;
    typedef const fmpz* const_rt;
};

struct fmpz_data
{
    fmpz_t f;

    fmpz_data() {fmpz_init(f);}
    ~fmpz_data() {fmpz_clear(f);}
    fmpz_data(const fmpz_data& o) {fmpz_init_set(f, o.f);}

    fmpz_data(const char* str)
    {
        fmpz_init(f);
        fmpz_set_str(f, str, 10);
    }

    template<class T>
    fmpz_data(const T& t)
    {
        init(t);
    }

    template<class T>
    typename mp::enable_if<traits::is_unsigned_integer<T> >::type init(T t)
    {
        fmpz_init_set_ui(f, t);
    }
    template<class T>
    typename mp::enable_if<traits::is_signed_integer<T> >::type init(T t)
    {
        fmpz_init(f);
        fmpz_set_si(f, t);
    }

    void init(const fmpzxx_cref&);
};

struct fmpzref_data
{
    fmpz* f;
    fmpzref_data(fmpzxx& r) {f = &r._fmpz()[0];}
};

struct fmpzcref_data
{
    const fmpz* f;
    fmpzcref_data(const fmpzxx& r) {f = r._fmpz();}
    fmpzcref_data(fmpzxx_ref r) {f = r._fmpz();}
};

inline void fmpz_data::init(const fmpzxx_cref& r)
{
    fmpz_init_set(f, r._fmpz());
}
} // detail

///////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////
namespace traits {
template<class T> struct is_fmpzxx : is_T_expr<T, fmpzxx> { };
} // traits
namespace mp {
template<class Out, class T1, class T2 = void>
struct enable_all_fmpzxx
    : mp::enable_if<mp::and_<traits::is_fmpzxx<T1>, traits::is_fmpzxx<T2> >, Out> { };
template<class Out, class T>
struct enable_all_fmpzxx<Out, T, void>
    : mp::enable_if<traits::is_fmpzxx<T>, Out> { };
} // mp

///////////////////////////////////////////////////////////////////
// RULES
///////////////////////////////////////////////////////////////////
namespace rules {

FLINT_DEFINE_DOIT(assignment, fmpzxx, fmpzxx, fmpz_set(to._fmpz(), from._fmpz()))

FLINT_DEFINE_DOIT_COND(assignment, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_set_ui(to._fmpz(), from))

FLINT_DEFINE_DOIT_COND(assignment, fmpzxx, traits::is_signed_integer<T>,
        fmpz_set_si(to._fmpz(), from))

template<int n>
struct assignment<fmpzxx, char[n]>
{
    static void doit(fmpzxx& target, const char* source)
    {
        fmpz_set_str(target._fmpz(), const_cast<char*>(source), 10);
    }
};

template<>
struct cmp<fmpzxx, fmpzxx>
{
    static int get(fmpzxx_cref l, fmpzxx_cref r)
    {
        return fmpz_cmp(l._fmpz(), r._fmpz());
    }
};

template<class T>
struct cmp<fmpzxx, T,
    typename mp::enable_if<traits::is_signed_integer<T> >::type>
{
    static int get(fmpzxx_cref v, const T& t)
    {
        return fmpz_cmp_si(v._fmpz(), t);
    }
};

template<class T>
struct cmp<fmpzxx, T,
    typename mp::enable_if<traits::is_unsigned_integer<T> >::type>
{
    static int get(fmpzxx_cref v, const T& t)
    {
        return fmpz_cmp_ui(v._fmpz(), t);
    }
};

template<>
struct to_string<fmpzxx>
{
    static std::string get(fmpzxx_cref v, int base)
    {
        char* str = fmpz_get_str(0, base, v._fmpz());
        std::string res(str);
        std::free(str);
        return res;
    }
};

FLINT_DEFINE_GET(conversion, slong, fmpzxx, fmpz_get_si(from._fmpz()))
FLINT_DEFINE_GET(conversion, ulong, fmpzxx, fmpz_get_ui(from._fmpz()))
FLINT_DEFINE_GET(conversion, double, fmpzxx, fmpz_get_d(from._fmpz()))

FLINT_DEFINE_BINARY_EXPR(plus, fmpzxx,
        fmpz_add(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_CBINARY_EXPR_COND(plus, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_add_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR(times, fmpzxx,
        fmpz_mul(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_CBINARY_EXPR_COND(times, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_mul_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_CBINARY_EXPR_COND(times, fmpzxx, traits::is_signed_integer<T>,
        fmpz_mul_si(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR(minus, fmpzxx,
        fmpz_sub(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND(minus, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_sub_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR(divided_by, fmpzxx,
        fmpz_fdiv_q(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND(divided_by, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_fdiv_q_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND(divided_by, fmpzxx, traits::is_signed_integer<T>,
        fmpz_fdiv_q_si(to._fmpz(), e1._fmpz(), e2))

// TODO this interpretation of mod is not the same as for builtin types!
FLINT_DEFINE_BINARY_EXPR(modulo, fmpzxx,
        fmpz_mod(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND(modulo, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_mod_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_UNARY_EXPR(negate, fmpzxx, fmpz_neg(to._fmpz(), from._fmpz()))


// Optimized evaluation rules using ternary arithmetic (addmul, submul)
// a +- b*c
template<class Op, class Left, class Right1, class Right2>
struct evaluation<Op,
    tuple<Left, tuple<
        fmpzxx_expression<operations::times,
            tuple<Right1, tuple<Right2, empty_tuple> > >,
        // NB: there is no particular reason to have the enable_if here,
        //     many other similar places would do
        typename mp::enable_if<mp::or_<
                mp::equal_types<Op, operations::plus>,
                mp::equal_types<Op, operations::minus> >,
            empty_tuple>::type> >,
    true, 1,
    typename tools::ternary_helper<fmpzxx, Left, Right1, Right2>::enable::type>
{
    // Helpful for testing.
    static const unsigned TERNARY_OP_MARKER = 0;

    typedef fmpzxx return_t;
    typedef tools::ternary_helper<fmpzxx, Left, Right1, Right2> th;
    typedef typename th::temporaries_t temporaries_t;
    typedef tuple<Left, tuple<
        fmpzxx_expression<operations::times,
            tuple<Right1, tuple<Right2, empty_tuple> > >,
        empty_tuple> > data_t;
    static const bool is_add = mp::equal_types<Op, operations::plus>::val;

    static void doit(const data_t& input, temporaries_t temps, return_t* res)
    {
        th t(input.first(), input.second()._data().first(),
                input.second()._data().second(), temps, res);
        if(is_add)
            fmpz_addmul(res->_fmpz(), t.left()._fmpz(), t.right()._fmpz());
        else
            fmpz_submul(res->_fmpz(), t.left()._fmpz(), t.right()._fmpz());
    }
};

// b*c + a
template<class Right, class Left1, class Left2>
struct evaluation<operations::plus,
    tuple<fmpzxx_expression<operations::times,
            tuple<Left1, tuple<Left2, empty_tuple> > >,
        tuple<Right, empty_tuple> >,
    true, 1,
    typename tools::ternary_helper<fmpzxx, 
        Right, Left1, Left2, operations::times>::enable::type>
{
    // Helpful for testing.
    static const unsigned TERNARY_OP_MARKER = 0;

    typedef fmpzxx return_t;
    typedef tools::ternary_helper<fmpzxx, Right, Left1, Left2> th;
    typedef typename th::temporaries_t temporaries_t;
    typedef tuple<fmpzxx_expression<operations::times,
            tuple<Left1, tuple<Left2, empty_tuple> > >,
        tuple<Right, empty_tuple> > data_t;

    static void doit(const data_t& input, temporaries_t temps, return_t* res)
    {
        th t(input.second(), input.first()._data().first(),
                input.first()._data().second(), temps, res);
        fmpz_addmul(res->_fmpz(), t.left()._fmpz(), t.right()._fmpz());
    }
};
} // rules

// Assignment-arithmetic using ternaries.
namespace detail {
template<class Right1, class Right2>
struct ternary_assign_helper
{
    typedef tools::evaluate_2<Right1, Right2> ev2_t;
    typedef typename ev2_t::temporaries_t temporaries_t;
    typedef mp::back_tuple<temporaries_t> back_t;

    typename back_t::type backing;
    ev2_t ev2;

    static temporaries_t backtemps(typename back_t::type& backing)
    {
        temporaries_t temps;
        back_t::init(temps, backing);
        return temps;
    }

    ternary_assign_helper(typename ev2_t::arg1_t r1, typename ev2_t::arg2_t r2)
        : backing(mp::htuples::fill<typename back_t::type>(
                    tools::temporaries_filler(r1+r2 /* XXX */))),
          ev2(backtemps(backing), r1, r2) {}
    const fmpzxx& getleft() {return ev2.get1();}
    const fmpzxx& getright() {return ev2.get2();}
};

template<class Right1, class Right2>
struct enable_ternary_assign
    : mp::enable_all_fmpzxx<fmpzxx&,
        typename traits::basetype<Right1>::type,
        typename traits::basetype<Right2>::type> { };
} // detail

// a += b*c
template<class Right1, class Right2>
inline typename detail::enable_ternary_assign<Right1, Right2>::type
operator+=(fmpzxx_ref left, const fmpzxx_expression<operations::times,
        tuple<Right1, tuple<Right2, empty_tuple> > >& other)
{
    detail::ternary_assign_helper<Right1, Right2> tah(
            other._data().first(), other._data().second());
    fmpz_addmul(left._fmpz(), tah.getleft()._fmpz(),
            tah.getright()._fmpz());
    return left;
}

// a -= b*c
template<class Right1, class Right2>
inline typename detail::enable_ternary_assign<Right1, Right2>::type
operator-=(fmpzxx_ref left, const fmpzxx_expression<operations::times,
        tuple<Right1, tuple<Right2, empty_tuple> > >& other)
{
    detail::ternary_assign_helper<Right1, Right2> tah(
            other._data().first(), other._data().second());
    fmpz_submul(left._fmpz(), tah.getleft()._fmpz(),
            tah.getright()._fmpz());
    return left;
}


///////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////

// These functions evaluate immediately and do not yield fmpzxxs

template<class T1, class T2>
inline typename mp::enable_all_fmpzxx<bool, T1, T2>::type
divisible(const T1& t1, const T2& t2)
{
    return fmpz_divisible(t1.evaluate()._fmpz(), t2.evaluate()._fmpz());
}
template<class T1, class T2>
inline typename mp::enable_if<mp::and_<
    traits::is_fmpzxx<T1>, traits::fits_into_slong<T2> >, bool>::type
divisible(const T1& t1, const T2& t2)
{
    return fmpz_divisible_si(t1.evaluate()._fmpz(), t2);
}

// These functions are evaluated lazily

FLINT_DEFINE_UNOP(fac)
FLINT_DEFINE_BINOP(rfac)
FLINT_DEFINE_BINOP(bin)
namespace rules {
FLINT_DEFINE_BINARY_EXPR_COND(rfac_op, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_rfac_ui(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_UNARY_EXPR_COND(fac_op, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_fac_ui(to._fmpz(), from))

template<class T1, class T2>
struct binary_expression<
    T1,
    typename mp::enable_if<
        mp::and_<
            traits::is_unsigned_integer<T1>,
            traits::is_unsigned_integer<T2> >,
        operations::bin_op>::type,
    T2>
{
    typedef fmpzxx return_t;
    static void doit(fmpzxx_ref to, const T1& t1, const T2& t2)
    {
        fmpz_bin_uiui(to._fmpz(), t1, t2);
    }
};

// standard math functions (c/f stdmath.h)
FLINT_DEFINE_BINARY_EXPR_COND(pow_op, fmpzxx, traits::is_unsigned_integer<T>,
        fmpz_pow_ui(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_BINARY_EXPR_COND(root_op, fmpzxx, traits::fits_into_slong<T>,
        fmpz_root(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_UNARY_EXPR(sqrt_op, fmpzxx, fmpz_sqrt(to._fmpz(), from._fmpz()))
} // rules

// TODO many more functions

} // flint

#endif