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

#ifndef NMOD_MATXX_H
#define NMOD_MATXX_H

#include <string>

#include "nmod_mat.h"

#include "nmod_vecxx.h"

#include "flintxx/flint_exception.h"
#include "flintxx/ltuple.h"
#include "flintxx/matrix.h"

// TODO printing
// TODO addmul
// TODO default argument for mat_solve_triu etc?
// TODO LU decomposition
// TODO nullspace member

namespace flint {
FLINT_DEFINE_BINOP(solve_vec)
FLINT_DEFINE_BINOP(mul_strassen)
FLINT_DEFINE_THREEARY(solve_tril)
FLINT_DEFINE_THREEARY(solve_tril_classical)
FLINT_DEFINE_THREEARY(solve_tril_recursive)
FLINT_DEFINE_THREEARY(solve_triu)
FLINT_DEFINE_THREEARY(solve_triu_classical)
FLINT_DEFINE_THREEARY(solve_triu_recursive)

namespace detail {
template<class Mat>
struct nmod_matxx_traits : matrices::generic_traits<Mat> { };
} // detail

template<class Operation, class Data>
class nmod_matxx_expression
    : public expression<derived_wrapper<nmod_matxx_expression>, Operation, Data>
{
public:
    typedef expression<derived_wrapper< ::flint::nmod_matxx_expression>,
              Operation, Data> base_t;
    typedef detail::nmod_matxx_traits<nmod_matxx_expression> traits_t;

    FLINTXX_DEFINE_BASICS(nmod_matxx_expression)
    FLINTXX_DEFINE_CTORS(nmod_matxx_expression)
    FLINTXX_DEFINE_C_REF(nmod_matxx_expression, nmod_mat_struct, _mat)

    // These only make sense with immediates
    nmodxx_ctx_srcref _ctx() const
        {return nmodxx_ctx_srcref::make(_mat()->mod);}

    // These work on any expression without evaluation
    nmodxx_ctx_srcref estimate_ctx() const
    {
        return tools::find_nmodxx_ctx(*this);
    }
    mp_limb_t modulus() const {return estimate_ctx().n();}

    template<class Expr>
    static evaluated_t create_temporary_rowscols(
            const Expr& e, slong rows, slong cols)
    {
        return evaluated_t(rows, cols, tools::find_nmodxx_ctx(e).n());
    }
    FLINTXX_DEFINE_MATRIX_METHODS(traits_t)

    // these only make sense with targets
    void set_randtest(frandxx& state)
        {nmod_mat_randtest(_mat(), state._data());}
    void set_randfull(frandxx& state)
        {nmod_mat_randfull(_mat(), state._data());}
    void set_randrank(frandxx& state, slong rank)
        {nmod_mat_randrank(_mat(), state._data(), rank);}
    void set_randtril(frandxx& state, bool unit)
        {nmod_mat_randtril(_mat(), state._data(), unit);}
    void set_randtriu(frandxx& state, bool unit)
        {nmod_mat_randtriu(_mat(), state._data(), unit);}

    template<class Vec>
    int set_randpermdiag(frandxx& state, const Vec& v)
    {
        return nmod_mat_randpermdiag(_mat(), state._data(), v._array(), v.size());
    }

    void apply_randops(frandxx& state, slong count)
        {nmod_mat_randops(_mat(), count, state._data());}

    slong set_rref() {return nmod_mat_rref(_mat());}

    // these cause evaluation
    slong rank() const {return nmod_mat_rank(this->evaluate()._mat());}
    bool is_zero() const {return nmod_mat_is_zero(this->evaluate()._mat());}
    bool is_empty() const {return nmod_mat_is_empty(this->evaluate()._mat());}
    bool is_square() const {return nmod_mat_is_square(this->evaluate()._mat());}

    // lazy members
    FLINTXX_DEFINE_MEMBER_BINOP(solve)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_classical)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_strassen)
    FLINTXX_DEFINE_MEMBER_UNOP(inv)
    FLINTXX_DEFINE_MEMBER_UNOP(transpose)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(nmodxx, trace)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(nmodxx, det)
    //FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(???, nullspace) // TODO
    FLINTXX_DEFINE_MEMBER_3OP(solve_tril)
    FLINTXX_DEFINE_MEMBER_3OP(solve_tril_recursive)
    FLINTXX_DEFINE_MEMBER_3OP(solve_tril_classical)
    FLINTXX_DEFINE_MEMBER_3OP(solve_triu)
    FLINTXX_DEFINE_MEMBER_3OP(solve_triu_recursive)
    FLINTXX_DEFINE_MEMBER_3OP(solve_triu_classical)
};

namespace detail {
struct nmod_mat_data;
} // detail

typedef nmod_matxx_expression<operations::immediate, detail::nmod_mat_data> nmod_matxx;
typedef nmod_matxx_expression<operations::immediate,
            flint_classes::ref_data<nmod_matxx, nmod_mat_struct> > nmod_matxx_ref;
typedef nmod_matxx_expression<operations::immediate, flint_classes::srcref_data<
    nmod_matxx, nmod_matxx_ref, nmod_mat_struct> > nmod_matxx_srcref;

template<>
struct matrix_traits<nmod_matxx>
{
    template<class M> static slong rows(const M& m)
    {
        return nmod_mat_nrows(m._mat());
    }
    template<class M> static slong cols(const M& m)
    {
        return nmod_mat_ncols(m._mat());
    }

    template<class M> static nmodxx_srcref at(const M& m, slong i, slong j)
    {
        return nmodxx_srcref::make(nmod_mat_entry(m._mat(), i, j),
                m.estimate_ctx());
    }
    template<class M> static nmodxx_ref at(M& m, slong i, slong j)
    {
        return nmodxx_ref::make(nmod_mat_entry(m._mat(), i, j),
                m.estimate_ctx());
    }
};

namespace traits {
template<> struct has_nmodxx_ctx<nmod_matxx> : mp::true_ { };
template<> struct has_nmodxx_ctx<nmod_matxx_ref> : mp::true_ { };
template<> struct has_nmodxx_ctx<nmod_matxx_srcref> : mp::true_ { };
} // traits

namespace detail {
template<>
struct nmod_matxx_traits<nmod_matxx_srcref>
    : matrices::generic_traits_srcref<nmod_matxx_srcref, nmodxx_srcref> { };

template<>
struct nmod_matxx_traits<nmod_matxx_ref>
    : matrices::generic_traits_ref<nmod_matxx_ref, nmodxx_ref, nmodxx_srcref> { };
template<> struct nmod_matxx_traits<nmod_matxx>
    : nmod_matxx_traits<nmod_matxx_ref> { };

struct nmod_mat_data
{
    typedef nmod_mat_t& data_ref_t;
    typedef const nmod_mat_t& data_srcref_t;

    nmod_mat_t inner;

    nmod_mat_data(slong m, slong n, mp_limb_t modulus)
    {
        nmod_mat_init(inner, m, n, modulus);
    }

    nmod_mat_data(const nmod_mat_data& o)
    {
        nmod_mat_init_set(inner, o.inner);
    }

    nmod_mat_data(nmod_matxx_srcref o)
    {
        nmod_mat_init_set(inner, o._data().inner);
    }

    ~nmod_mat_data() {nmod_mat_clear(inner);}
};
} // detail

namespace matrices {
template<>
struct outsize<operations::mul_strassen_op>
    : outsize<operations::times> { };

template<> struct outsize<operations::solve_tril_op>
    : outsize<operations::solve_op> { };
template<> struct outsize<operations::solve_tril_classical_op>
    : outsize<operations::solve_op> { };
template<> struct outsize<operations::solve_tril_recursive_op>
    : outsize<operations::solve_op> { };
template<> struct outsize<operations::solve_triu_op>
    : outsize<operations::solve_op> { };
template<> struct outsize<operations::solve_triu_classical_op>
    : outsize<operations::solve_op> { };
template<> struct outsize<operations::solve_triu_recursive_op>
    : outsize<operations::solve_op> { };
}

// temporary instantiation stuff
FLINTXX_DEFINE_TEMPORARY_RULES(nmod_matxx)

#define NMOD_MATXX_COND_S FLINTXX_COND_S(nmod_matxx)
#define NMOD_MATXX_COND_T FLINTXX_COND_T(nmod_matxx)

namespace traits {
template<class T> struct is_nmod_matxx
    : flint_classes::is_Base<nmod_matxx, T> { };
} // traits

namespace rules {
FLINT_DEFINE_DOIT_COND2(assignment, NMOD_MATXX_COND_T, NMOD_MATXX_COND_S,
        nmod_mat_set(to._mat(), from._mat()))

FLINTXX_DEFINE_SWAP(nmod_matxx, nmod_mat_swap(e1._mat(), e2._mat()))

FLINTXX_DEFINE_EQUALS(nmod_matxx, nmod_mat_equal(e1._mat(), e2._mat()))

FLINT_DEFINE_THREEARY_EXPR_COND3(mat_at_op, nmodxx,
        NMOD_MATXX_COND_S, traits::fits_into_slong, traits::fits_into_slong,
        to.set_nored(nmod_mat_entry(e1._mat(), e2, e3)))

FLINT_DEFINE_BINARY_EXPR_COND2(times, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        nmod_mat_mul(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_CBINARY_EXPR_COND2(times, nmod_matxx,
        NMOD_MATXX_COND_S, NMODXX_COND_S,
        nmod_mat_scalar_mul(to._mat(), e1._mat(), e2._limb()))

FLINT_DEFINE_BINARY_EXPR_COND2(plus, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        nmod_mat_add(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(minus, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        nmod_mat_sub(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(negate, nmod_matxx, NMOD_MATXX_COND_S,
        nmod_mat_neg(to._mat(), from._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(transpose_op, nmod_matxx, NMOD_MATXX_COND_S,
        nmod_mat_transpose(to._mat(), from._mat()))
FLINT_DEFINE_UNARY_EXPR_COND(trace_op, nmodxx, NMOD_MATXX_COND_S,
        to.set_nored(nmod_mat_trace(from._mat())))

FLINT_DEFINE_BINARY_EXPR_COND2(mul_classical_op, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        nmod_mat_mul(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_strassen_op, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        nmod_mat_mul(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(det_op, nmodxx, NMOD_MATXX_COND_S,
        to.set_nored(nmod_mat_det(from._mat())))

FLINT_DEFINE_UNARY_EXPR_COND(inv_op, nmod_matxx, NMOD_MATXX_COND_S,
        execution_check(nmod_mat_inv(to._mat(), from._mat()),
            "inv", "nmod_mat"))

#define NMOD_MATXX_DEFINE_SOLVE_TRI(name) \
FLINT_DEFINE_THREEARY_EXPR_COND3(name##_op, nmod_matxx, \
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S, tools::is_bool, \
        nmod_mat_##name(to._mat(), e1._mat(), e2._mat(), e3))
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_tril)
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_tril_classical)
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_tril_recursive)
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_triu)
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_triu_classical)
NMOD_MATXX_DEFINE_SOLVE_TRI(solve_triu_recursive)

FLINT_DEFINE_BINARY_EXPR_COND2(solve_op, nmod_matxx,
        NMOD_MATXX_COND_S, NMOD_MATXX_COND_S,
        execution_check(nmod_mat_solve(to._mat(), e1._mat(), e2._mat()),
            "solve", "nmod_mat"))

FLINT_DEFINE_BINARY_EXPR_COND2(solve_op, nmod_vecxx,
        NMOD_MATXX_COND_S, NMOD_VECXX_COND_S,
        execution_check(nmod_mat_solve_vec(to._array(), e1._mat(), e2._array()),
            "solve_vec", "nmod_mat"))

namespace rdetail {
typedef make_ltuple<mp::make_tuple<slong, nmod_matxx>::type >::type
    nmod_mat_nullspace_rt;
} // rdetail
FLINT_DEFINE_UNARY_EXPR_COND(nullspace_op, rdetail::nmod_mat_nullspace_rt,
        NMOD_MATXX_COND_S, to.template get<0>() = nmod_mat_nullspace(
            to.template get<1>()._mat(), from._mat()))
} // rules
} // flint

#endif