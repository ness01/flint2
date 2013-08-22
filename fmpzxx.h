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

#include "flintxx/evaluation_tools.h"
#include "flintxx/expression.h"
#include "flintxx/expression_traits.h"
#include "flintxx/flint_classes.h"
#include "flintxx/frandxx.h"
#include "flintxx/ltuple.h"
#include "flintxx/stdmath.h"

#include "fmpz.h"

// TODO bit packing
// TODO chinese remaindering
// TODO functions for addmul? inhomogeneous addmul?
// TODO use evaluate_n in immediate functions?

namespace flint {
FLINT_DEFINE_BINOP(cdiv_q)
FLINT_DEFINE_BINOP(fdiv_r)
FLINT_DEFINE_BINOP(tdiv_q)
FLINT_DEFINE_BINOP(fdiv_r_2exp)
FLINT_DEFINE_BINOP(tdiv_q_2exp)
FLINT_DEFINE_UNOP(fac)
FLINT_DEFINE_UNOP(fib)
FLINT_DEFINE_BINOP(rfac)
FLINT_DEFINE_BINOP(bin)
FLINT_DEFINE_BINOP(gcd)
FLINT_DEFINE_BINOP(lcm)
FLINT_DEFINE_BINOP(invmod)
FLINT_DEFINE_BINOP(negmod)
FLINT_DEFINE_THREEARY(mul2)
FLINT_DEFINE_THREEARY(divexact2)
FLINT_DEFINE_THREEARY(powm)
FLINT_DEFINE_THREEARY(mul_tdiv_q_2exp)
FLINT_DEFINE_BINOP(fdiv_qr)
FLINT_DEFINE_BINOP(tdiv_qr)
FLINT_DEFINE_BINOP(sqrtmod)
FLINT_DEFINE_UNOP(sqrtrem)
FLINT_DEFINE_BINOP(gcdinv)
FLINT_DEFINE_BINOP(xgcd)

namespace mp {
template<class Out, class T1, class T2 = void, class T3 = void, class T4 = void>
struct enable_all_fmpzxx;
}

template<class Operation, class Data>
class fmpzxx_expression
    : public expression<derived_wrapper<fmpzxx_expression>, Operation, Data>
{
public:
    typedef expression<derived_wrapper< ::flint::fmpzxx_expression>,
              Operation, Data> base_t;

    FLINTXX_DEFINE_BASICS(fmpzxx_expression)
    FLINTXX_DEFINE_CTORS(fmpzxx_expression)
    FLINTXX_DEFINE_C_REF(fmpzxx_expression, fmpz, _fmpz)

    // these only make sense with fmpzxx
    FLINTXX_DEFINE_RANDFUNC(fmpz, randbits)
    FLINTXX_DEFINE_RANDFUNC(fmpz, randtest)
    FLINTXX_DEFINE_RANDFUNC(fmpz, randtest_unsigned)
    FLINTXX_DEFINE_RANDFUNC(fmpz, randtest_not_zero)

    template<class Fmpz>
    static fmpzxx_expression randm(frandxx& state, const Fmpz& m)
    {
        fmpzxx_expression res;
        fmpz_randm(res._fmpz(), state._data(), m.evaluate()._fmpz());
        return res;
    }
    template<class Fmpz>
    static fmpzxx_expression randtest_mod(frandxx& state, const Fmpz& m)
    {
        fmpzxx_expression res;
        fmpz_randtest_mod(res._fmpz(), state._data(), m.evaluate()._fmpz());
        return res;
    }
    template<class Fmpz>
    static fmpzxx_expression randtest_mod_signed(frandxx& state, const Fmpz& m)
    {
        fmpzxx_expression res;
        fmpz_randtest_mod_signed(res._fmpz(), state._data(), m.evaluate()._fmpz());
        return res;
    }

    template<class Fmpz_target, class Fmpz_expr1, class Fmpz_expr2>
    static slong remove(Fmpz_target& rop,
            const Fmpz_expr1& op, const Fmpz_expr2& f)
    {
        return fmpz_remove(rop._fmpz(), op.evaluate()._fmpz(),
                f.evaluate()._fmpz());
    }

    // TODO would these make more sense static?
    void set_ui_smod(mp_limb_t x, mp_limb_t m)
    {
        fmpz_set_ui_smod(this->_fmpz(), x, m);
    }
    void set_uiui(mp_limb_t hi, mp_limb_t lo)
    {
        fmpz_set_uiui(this->_fmpz(), hi, lo);
    }
    void neg_uiui(mp_limb_t hi, mp_limb_t lo)
    {
        fmpz_neg_uiui(this->_fmpz(), hi, lo);
    }
    // these only make sense with fmpzxx/fmpzxx_ref
    void clrbit(ulong i) {fmpz_clrbit(_fmpz(), i);}
    void combit(ulong i) {fmpz_combit(_fmpz(), i);}

    // These make sense with all expressions, but cause evaluation
    double get_d_2exp(long& exp) const
    {
        return fmpz_get_d_2exp(&exp, this->evaluate()._fmpz());
    }
    bool is_zero() const
    {
        return fmpz_is_zero(this->evaluate()._fmpz());
    }
    bool is_one() const
    {
        return fmpz_is_one(this->evaluate()._fmpz());
    }
    bool is_pm1() const
    {
        return fmpz_is_pm1(this->evaluate()._fmpz());
    }
    bool is_even() const
    {
        return fmpz_is_even(this->evaluate()._fmpz());
    }
    bool is_odd() const
    {
        return fmpz_is_odd(this->evaluate()._fmpz());
    }
    bool is_square() const
    {
        return fmpz_is_square(this->evaluate()._fmpz());
    }
    int popcnt() const
    {
        return fmpz_popcnt(this->evaluate()._fmpz());
    }
    bool is_probabprime() const
    {
        return fmpz_is_probabprime(this->evaluate()._fmpz());
    }
    bool is_prime_pseudosquare() const
    {
        return fmpz_is_prime_pseudosquare(this->evaluate()._fmpz());
    }

    template<class T2>
    typename mp::enable_all_fmpzxx<bool, T2>::type
    divisible(const T2& t2) const
    {
        return fmpz_divisible(this->evaluate()._fmpz(), t2.evaluate()._fmpz());
    }
    template<class T2>
    typename mp::enable_if<traits::fits_into_slong<T2>, bool>::type
    divisible(const T2& t2) const
    {
        return fmpz_divisible_si(this->evaluate()._fmpz(), t2);
    }
    template<class Fmpz2>
    typename mp::enable_all_fmpzxx<long, Fmpz2>::type
    clog(const Fmpz2& b) const
    {
        return fmpz_clog(this->evaluate()._fmpz(), b.evaluate()._fmpz());
    }
    template<class Int>
    typename mp::enable_if<traits::is_unsigned_integer<Int>, long>::type
    clog(Int b) const
    {
        return fmpz_clog_ui(this->evaluate()._fmpz(), b);
    }
    template<class Fmpz2>
    typename mp::enable_all_fmpzxx<long, Fmpz2>::type
    flog(const Fmpz2& b) const
    {
        return fmpz_flog(this->evaluate()._fmpz(), b.evaluate()._fmpz());
    }
    template<class Int>
    typename mp::enable_if<traits::is_unsigned_integer<Int>, long>::type
    flog(Int b) const
    {
        return fmpz_flog_ui(this->evaluate()._fmpz(), b);
    }
    double dlog() const
    {
        return fmpz_dlog(this->evaluate()._fmpz());
    }
    template<class Fmpz2>
    typename mp::enable_all_fmpzxx<int, Fmpz2>::type
    jacobi(const Fmpz2& p) const
    {
        return fmpz_jacobi(this->evaluate()._fmpz(), p.evaluate()._fmpz());
    }

    // lazy function forwarding
    FLINTXX_DEFINE_MEMBER_3OP(divexact2)
    FLINTXX_DEFINE_MEMBER_3OP(mul2)
    FLINTXX_DEFINE_MEMBER_3OP(mul_tdiv_q_2exp)
    FLINTXX_DEFINE_MEMBER_3OP(powm)
    FLINTXX_DEFINE_MEMBER_BINOP(cdiv_q)
    FLINTXX_DEFINE_MEMBER_BINOP(divexact)
    FLINTXX_DEFINE_MEMBER_BINOP(fdiv_qr)
    FLINTXX_DEFINE_MEMBER_BINOP(fdiv_r)
    FLINTXX_DEFINE_MEMBER_BINOP(fdiv_r_2exp)
    FLINTXX_DEFINE_MEMBER_BINOP(gcd)
    FLINTXX_DEFINE_MEMBER_BINOP(gcdinv)
    FLINTXX_DEFINE_MEMBER_BINOP(invmod)
    FLINTXX_DEFINE_MEMBER_BINOP(lcm)
    FLINTXX_DEFINE_MEMBER_BINOP(negmod)
    FLINTXX_DEFINE_MEMBER_BINOP(pow)
    FLINTXX_DEFINE_MEMBER_BINOP(rfac)
    FLINTXX_DEFINE_MEMBER_BINOP(root)
    FLINTXX_DEFINE_MEMBER_BINOP(sqrtmod)
    FLINTXX_DEFINE_MEMBER_BINOP(tdiv_q)
    FLINTXX_DEFINE_MEMBER_BINOP(tdiv_q_2exp)
    FLINTXX_DEFINE_MEMBER_BINOP(tdiv_qr)
    FLINTXX_DEFINE_MEMBER_BINOP(xgcd)
    FLINTXX_DEFINE_MEMBER_UNOP(abs)
    FLINTXX_DEFINE_MEMBER_UNOP(sqrt)
    // FLINTXX_DEFINE_MEMBER_UNOP(sqrtrem) // TODO
};

namespace detail {
struct fmpz_data;
}

typedef fmpzxx_expression<operations::immediate, detail::fmpz_data> fmpzxx;
typedef fmpzxx_expression<operations::immediate,
            flint_classes::ref_data<fmpzxx, fmpz> > fmpzxx_ref;
typedef fmpzxx_expression<operations::immediate,
            flint_classes::srcref_data<fmpzxx, fmpzxx_ref, fmpz> > fmpzxx_srcref;

namespace detail {
struct fmpz_data
{
    typedef fmpz_t& data_ref_t;
    typedef const fmpz_t& data_srcref_t;

    fmpz_t inner;

    fmpz_data() {fmpz_init(inner);}
    ~fmpz_data() {fmpz_clear(inner);}
    fmpz_data(const fmpz_data& o) {fmpz_init_set(inner, o.inner);}

    fmpz_data(const char* str)
    {
        fmpz_init(inner);
        fmpz_set_str(inner, str, 10);
    }

    template<class T>
    fmpz_data(const T& t)
    {
        init(t);
    }

    template<class T>
    typename mp::enable_if<traits::is_unsigned_integer<T> >::type init(T t)
    {
        fmpz_init_set_ui(inner, t);
    }
    template<class T>
    typename mp::enable_if<traits::is_signed_integer<T> >::type init(T t)
    {
        fmpz_init(inner);
        fmpz_set_si(inner, t);
    }

    void init(const fmpzxx_srcref& r)
    {
        fmpz_init_set(inner, r._fmpz());
    }
};

} // detail

///////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////
namespace traits {
template<class T> struct is_fmpzxx : mp::or_<
     traits::is_T_expr<T, fmpzxx>,
     flint_classes::is_source<fmpzxx, T> > { };
} // traits
namespace mp {
template<class T1, class T2 = void, class T3 = void, class T4 = void>
struct all_fmpzxx : mp::and_<all_fmpzxx<T1>, all_fmpzxx<T2, T3, T4> > { };
template<class T>
struct all_fmpzxx<T, void, void, void> : traits::is_fmpzxx<T> { };

template<class Out, class T1, class T2, class T3, class T4>
struct enable_all_fmpzxx
    : mp::enable_if<all_fmpzxx<T1, T2, T3, T4>, Out> { };
} // mp

///////////////////////////////////////////////////////////////////
// RULES
///////////////////////////////////////////////////////////////////
namespace rules {

#define FMPZXX_COND_S FLINTXX_COND_S(fmpzxx)
#define FMPZXX_COND_T FLINTXX_COND_T(fmpzxx)

FLINT_DEFINE_DOIT_COND2(assignment, FMPZXX_COND_T, FMPZXX_COND_S,
        fmpz_set(to._fmpz(), from._fmpz()))

FLINT_DEFINE_DOIT_COND2(assignment,
        FMPZXX_COND_T, traits::is_unsigned_integer,
        fmpz_set_ui(to._fmpz(), from))

FLINT_DEFINE_DOIT_COND2(assignment,
        FMPZXX_COND_T, traits::is_signed_integer,
        fmpz_set_si(to._fmpz(), from))

FLINTXX_DEFINE_CMP(fmpzxx, fmpz_cmp(e1._fmpz(), e2._fmpz()))

template<class T, class U>
struct cmp<T, U,
    typename mp::enable_if<mp::and_<
        FMPZXX_COND_S<T>, traits::is_signed_integer<U> > >::type>
{
    static int get(const T& v, const U& t)
    {
        return fmpz_cmp_si(v._fmpz(), t);
    }
};

FLINTXX_DEFINE_ASSIGN_STR(fmpzxx, fmpz_set_str(to._fmpz(), from, 10))

template<class T>
struct cmp<fmpzxx, T,
    typename mp::enable_if<traits::is_unsigned_integer<T> >::type>
{
    static int get(const fmpzxx& v, const T& t)
    {
        return fmpz_cmp_ui(v._fmpz(), t);
    }
};

FLINTXX_DEFINE_TO_STR(fmpzxx, fmpz_get_str(0,  base, from._fmpz()))

FLINTXX_DEFINE_SWAP(fmpzxx, fmpz_swap(e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_GET_COND(conversion, slong, FMPZXX_COND_S,
        fmpz_get_si(from._fmpz()))
FLINT_DEFINE_GET_COND(conversion, ulong, FMPZXX_COND_S,
        fmpz_get_ui(from._fmpz()))
FLINT_DEFINE_GET_COND(conversion, double, FMPZXX_COND_S,
        fmpz_get_d(from._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(plus, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_add(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_CBINARY_EXPR_COND2(plus, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_add_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(times, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_mul(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_mul_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpzxx,
        FMPZXX_COND_S, traits::is_signed_integer,
        fmpz_mul_si(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(minus, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_sub(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(minus, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_sub_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(divided_by, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_fdiv_q(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(divided_by, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_fdiv_q_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(divided_by, fmpzxx,
        FMPZXX_COND_S, traits::is_signed_integer,
        fmpz_fdiv_q_si(to._fmpz(), e1._fmpz(), e2))

// TODO this interpretation of mod is not the same as for builtin types!
FLINT_DEFINE_BINARY_EXPR_COND2(modulo, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_mod(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(modulo, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_mod_ui(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(binary_and, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_and(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(binary_or, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_or(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(binary_xor, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_xor(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_UNARY_EXPR_COND(negate, fmpzxx, FMPZXX_COND_S,
        fmpz_neg(to._fmpz(), from._fmpz()))

FLINT_DEFINE_UNARY_EXPR_COND(complement, fmpzxx, FMPZXX_COND_S,
        fmpz_complement(to._fmpz(), from._fmpz()))

namespace rdetail {
template<class Fmpz1, class Fmpz2, class T>
void fmpzxx_shift(Fmpz1& to, const Fmpz2& from, T howmuch)
{
    if(howmuch < 0)
        fmpz_fdiv_q_2exp(to._fmpz(), from._fmpz(), -howmuch);
    else
        fmpz_mul_2exp(to._fmpz(), from._fmpz(), howmuch);
}
} // rdetail
FLINT_DEFINE_BINARY_EXPR_COND2(shift, fmpzxx,
        FMPZXX_COND_S, traits::is_integer,
        rdetail::fmpzxx_shift(to, e1, e2))
} // rules

FLINTXX_DEFINE_TERNARY(fmpzxx,
        fmpz_addmul(to._fmpz(), e1._fmpz(), e2._fmpz()),
        fmpz_submul(to._fmpz(), e1._fmpz(), e2._fmpz()),
        FLINTXX_UNADORNED_MAKETYPES)

///////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////

// These functions evaluate immediately, and (often) do not yield fmpzxxs

template<class T1, class T2>
inline typename mp::enable_all_fmpzxx<bool, T1>::type
divisible(const T1& t1, const T2& t2)
{
    return t1.divisible(t2);
}

template<class Fmpz1, class Fmpz2>
inline typename mp::enable_all_fmpzxx<long, Fmpz1>::type
clog(const Fmpz1& x, const Fmpz2& b)
{
    return x.clog(b);
}

template<class Fmpz1, class Fmpz2>
inline typename mp::enable_all_fmpzxx<long, Fmpz1>::type
flog(const Fmpz1& x, const Fmpz2& b)
{
    return x.flog(b);
}

template<class Fmpz>
inline typename mp::enable_if<traits::is_fmpzxx<Fmpz>, double>::type
dlog(const Fmpz& x)
{
    return x.dlog();
}

template<class Fmpz1, class Fmpz2>
inline typename mp::enable_all_fmpzxx<int, Fmpz1>::type
jacobi(const Fmpz1& a, const Fmpz2& p)
{
    return a.jacobi(p);
}

// These functions are evaluated lazily

// TODO move some of these to stdmath?
namespace rules {
FLINT_DEFINE_BINARY_EXPR_COND2(rfac_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_rfac_ui(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_UNARY_EXPR_COND(fac_op, fmpzxx, traits::is_unsigned_integer,
        fmpz_fac_ui(to._fmpz(), from))
FLINT_DEFINE_UNARY_EXPR_COND(fib_op, fmpzxx, traits::is_unsigned_integer,
        fmpz_fib_ui(to._fmpz(), from))
FLINT_DEFINE_BINARY_EXPR_COND2(gcd_op, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_gcd(to._fmpz(), e1._fmpz(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(lcm_op, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_lcm(to._fmpz(), e1._fmpz(), e2._fmpz()))

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
    template<class V>
    static void doit(V& to, const T1& t1, const T2& t2)
    {
        fmpz_bin_uiui(to._fmpz(), t1, t2);
    }
};

#define FMPZXX_DEFINE_DIVFUNCS(name) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S, \
        fmpz_##name(to._fmpz(), e1._fmpz(), e2._fmpz())) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, fmpzxx, FMPZXX_COND_S, \
        traits::is_signed_integer, \
        fmpz_##name##_si(to._fmpz(), e1._fmpz(), e2)) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, fmpzxx, FMPZXX_COND_S, \
        traits::is_unsigned_integer, \
        fmpz_##name##_ui(to._fmpz(), e1._fmpz(), e2))

FMPZXX_DEFINE_DIVFUNCS(cdiv_q)
FMPZXX_DEFINE_DIVFUNCS(tdiv_q)
FMPZXX_DEFINE_DIVFUNCS(divexact)
FLINT_DEFINE_BINARY_EXPR_COND2(fdiv_r_op, fmpzxx, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_fdiv_r(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(tdiv_q_2exp_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_tdiv_q_2exp(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(fdiv_r_2exp_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_fdiv_r_2exp(to._fmpz(), e1._fmpz(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(invmod_op, fmpzxx,
        FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_invmod(to._fmpz(), e1._fmpz(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(negmod_op, fmpzxx,
        FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_negmod(to._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_THREEARY_EXPR_COND3(mul2_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer, traits::is_unsigned_integer,
        fmpz_mul2_uiui(to._fmpz(), e1._fmpz(), e2, e3))
FLINT_DEFINE_THREEARY_EXPR_COND3(divexact2_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer, traits::is_unsigned_integer,
        fmpz_divexact2_uiui(to._fmpz(), e1._fmpz(), e2, e3))

FLINT_DEFINE_THREEARY_EXPR_COND3(powm_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer, FMPZXX_COND_S,
        fmpz_powm_ui(to._fmpz(), e1._fmpz(), e2, e3._fmpz()))
FLINT_DEFINE_THREEARY_EXPR_COND3(powm_op, fmpzxx,
        FMPZXX_COND_S, FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_powm(to._fmpz(), e1._fmpz(), e2._fmpz(), e3._fmpz()))

FLINT_DEFINE_THREEARY_EXPR_COND3(mul_tdiv_q_2exp_op, fmpzxx,
        FMPZXX_COND_S, FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_mul_tdiv_q_2exp(to._fmpz(), e1._fmpz(), e2._fmpz(), e3))
FLINT_DEFINE_THREEARY_EXPR_COND3(mul_tdiv_q_2exp_op, fmpzxx,
        FMPZXX_COND_S, traits::fits_into_slong, traits::is_unsigned_integer,
        fmpz_mul_si_tdiv_q_2exp(to._fmpz(), e1._fmpz(), e2, e3))
// TODO addmul, submul?

namespace rdetail {
typedef make_ltuple<mp::make_tuple<fmpzxx, fmpzxx>::type>::type fmpzxx_pair;
typedef make_ltuple<mp::make_tuple<fmpzxx, fmpzxx, fmpzxx>::type>::type fmpzxx_triple;
typedef make_ltuple<mp::make_tuple<bool, fmpzxx>::type>::type
    bool_fmpzxx_pair;
} // rdetail

FLINT_DEFINE_BINARY_EXPR_COND2(fdiv_qr_op, rdetail::fmpzxx_pair,
        FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_fdiv_qr(to.template get<0>()._fmpz(), to.template get<1>()._fmpz(),
            e1._fmpz(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(tdiv_qr_op, rdetail::fmpzxx_pair,
        FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_tdiv_qr(to.template get<0>()._fmpz(), to.template get<1>()._fmpz(),
            e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(sqrtmod_op, rdetail::bool_fmpzxx_pair,
        FMPZXX_COND_S, FMPZXX_COND_S,
        to.template get<0>() = fmpz_sqrtmod(
            to.template get<1>()._fmpz(), e1._fmpz(), e2._fmpz()))

FLINT_DEFINE_UNARY_EXPR_COND(sqrtrem_op, rdetail::fmpzxx_pair,
        FMPZXX_COND_S,
        fmpz_sqrtrem(to.template get<0>()._fmpz(),  to.template get<1>()._fmpz(),
            from._fmpz()))

FLINT_DEFINE_BINARY_EXPR_COND2(gcdinv_op, rdetail::fmpzxx_pair,
        FMPZXX_COND_S, FMPZXX_COND_S,
        fmpz_gcdinv(to.template get<0>()._fmpz(), to.template get<1>()._fmpz(),
            e1._fmpz(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(xgcd_op, rdetail::fmpzxx_triple,
        FMPZXX_COND_S, FMPZXX_COND_S, fmpz_xgcd(
            to.template get<0>()._fmpz(), to.template get<1>()._fmpz(),
            to.template get<2>()._fmpz(), e1._fmpz(), e2._fmpz()))

// standard math functions (c/f stdmath.h)
FLINT_DEFINE_BINARY_EXPR_COND2(pow_op, fmpzxx,
        FMPZXX_COND_S, traits::is_unsigned_integer,
        fmpz_pow_ui(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(root_op, fmpzxx,
        FMPZXX_COND_S, traits::fits_into_slong,
        fmpz_root(to._fmpz(), e1._fmpz(), e2))
FLINT_DEFINE_UNARY_EXPR_COND(sqrt_op, fmpzxx, FMPZXX_COND_S,
        fmpz_sqrt(to._fmpz(), from._fmpz()))
FLINT_DEFINE_UNARY_EXPR_COND(abs_op, fmpzxx, FMPZXX_COND_S,
        fmpz_abs(to._fmpz(), from._fmpz()))
} // rules

} // flint

#endif