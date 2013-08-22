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

#ifndef FMPQ_MATXX_H
#define FMPQ_MATXX_H FMPQ_MATXX_H

#include "fmpq_mat.h"

#include "fmpqxx.h"
#include "fmpz_matxx.h"

#include "flintxx/ltuple.h"
#include "flintxx/matrix.h"

// TODO wrap entry_num, entry_den?
// TODO input and output
// TODO fmpz_mat conversion
// TODO modular reduction
// TODO pivoting and row reduction

namespace flint {
FLINT_DEFINE_BINOP(hilbert_matrix)
FLINT_DEFINE_BINOP(mul_direct)
FLINT_DEFINE_BINOP(mul_cleared)
FLINT_DEFINE_BINOP(solve_fraction_free)

namespace detail {
template<class Mat>
struct fmpq_matxx_traits : matrices::generic_traits<Mat> { };
} // detail

template<class Operation, class Data>
class fmpq_matxx_expression
    : public expression<derived_wrapper<fmpq_matxx_expression>, Operation, Data>
{
public:
    typedef expression<derived_wrapper< ::flint::fmpq_matxx_expression>,
              Operation, Data> base_t;
    typedef detail::fmpq_matxx_traits<fmpq_matxx_expression> traits_t;

    FLINTXX_DEFINE_BASICS(fmpq_matxx_expression)
    FLINTXX_DEFINE_CTORS(fmpq_matxx_expression)
    FLINTXX_DEFINE_C_REF(fmpq_matxx_expression, fmpq_mat_struct, _mat)

    template<class Expr>
    static evaluated_t create_temporary_rowscols(
            const Expr&, slong rows, slong cols)
    {
        return evaluated_t(rows, cols);
    }
    FLINTXX_DEFINE_MATRIX_METHODS(traits_t)

    // these only make sense with targets
    void set_randbits(frandxx& state, mp_bitcnt_t bits)
        {fmpq_mat_randbits(_mat(), state._data(), bits);}
    void set_randtest(frandxx& state, mp_bitcnt_t bits)
        {fmpq_mat_randtest(_mat(), state._data(), bits);}
    void set_hilbert_matrix()
        {fmpq_mat_hilbert_matrix(_mat());}

    // these cause evaluation
    slong rank() const {return fmpq_mat_rank(this->evaluate()._mat());}
    bool is_zero() const {return fmpq_mat_is_zero(this->evaluate()._mat());}
    bool is_empty() const {return fmpq_mat_is_empty(this->evaluate()._mat());}
    bool is_square() const {return fmpq_mat_is_square(this->evaluate()._mat());}
    bool is_integral() const
        {return fmpq_mat_is_integral(this->evaluate()._mat());}

    // forwarded lazy ops
    FLINTXX_DEFINE_MEMBER_UNOP(inv)
    FLINTXX_DEFINE_MEMBER_UNOP(transpose)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpqxx, det)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpqxx, trace)

    FLINTXX_DEFINE_MEMBER_BINOP(mul_cleared)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_direct)
    FLINTXX_DEFINE_MEMBER_BINOP(solve_dixon)
    FLINTXX_DEFINE_MEMBER_BINOP(solve_fraction_free)
};

namespace detail {
struct fmpq_mat_data;
} // detail

typedef fmpq_matxx_expression<operations::immediate, detail::fmpq_mat_data> fmpq_matxx;
typedef fmpq_matxx_expression<operations::immediate,
            flint_classes::ref_data<fmpq_matxx, fmpq_mat_struct> > fmpq_matxx_ref;
typedef fmpq_matxx_expression<operations::immediate, flint_classes::srcref_data<
    fmpq_matxx, fmpq_matxx_ref, fmpq_mat_struct> > fmpq_matxx_srcref;

template<>
struct matrix_traits<fmpq_matxx>
{
    template<class M> static slong rows(const M& m)
    {
        return fmpq_mat_nrows(m._mat());
    }
    template<class M> static slong cols(const M& m)
    {
        return fmpq_mat_ncols(m._mat());
    }

    template<class M> static fmpqxx_srcref at(const M& m, slong i, slong j)
    {
        return fmpqxx_srcref::make(fmpq_mat_entry(m._mat(), i, j));
    }
    template<class M> static fmpqxx_ref at(M& m, slong i, slong j)
    {
        return fmpqxx_ref::make(fmpq_mat_entry(m._mat(), i, j));
    }
};

namespace detail {
template<>
struct fmpq_matxx_traits<fmpq_matxx_srcref>
    : matrices::generic_traits_srcref<fmpq_matxx_srcref, fmpqxx_srcref> { };

template<>
struct fmpq_matxx_traits<fmpq_matxx_ref>
    : matrices::generic_traits_ref<fmpq_matxx_ref, fmpqxx_ref, fmpqxx_srcref> { };
template<> struct fmpq_matxx_traits<fmpq_matxx>
    : fmpq_matxx_traits<fmpq_matxx_ref> { };

struct fmpq_mat_data
{
    typedef fmpq_mat_t& data_ref_t;
    typedef const fmpq_mat_t& data_srcref_t;

    fmpq_mat_t inner;

    fmpq_mat_data(slong m, slong n)
    {
        fmpq_mat_init(inner, m, n);
    }

    fmpq_mat_data(const fmpq_mat_data& o)
    {
        fmpq_mat_init(inner, fmpq_mat_nrows(o.inner), fmpq_mat_ncols(o.inner));
        fmpq_mat_set(inner, o.inner);
    }

    fmpq_mat_data(fmpq_matxx_srcref o)
    {
        fmpq_mat_init(inner, o.rows(), o.cols());
        fmpq_mat_set(inner, o._data().inner);
    }

    ~fmpq_mat_data() {fmpq_mat_clear(inner);}
};
} // detail

#define FMPQ_MATXX_COND_S FLINTXX_COND_S(fmpq_matxx)
#define FMPQ_MATXX_COND_T FLINTXX_COND_T(fmpq_matxx)

namespace traits {
template<class T> struct is_fmpq_matxx
    : flint_classes::is_Base<fmpq_matxx, T> { };
} // traits
namespace mp {
template<class T1, class T2 = void, class T3 = void, class T4 = void>
struct all_fmpq_matxx : mp::and_<all_fmpq_matxx<T1>, all_fmpq_matxx<T2, T3, T4> > { };
template<class T>
struct all_fmpq_matxx<T, void, void, void> : traits::is_fmpq_matxx<T> { };

template<class Out, class T1, class T2 = void, class T3 = void, class T4 = void>
struct enable_all_fmpq_matxx
    : mp::enable_if<all_fmpq_matxx<T1, T2, T3, T4>, Out> { };
} // mp

namespace matrices {
template<>
struct outsize<operations::mul_direct_op>
    : outsize<operations::times> { };
template<>
struct outsize<operations::mul_cleared_op>
    : outsize<operations::times> { };

template<> struct outsize<operations::solve_fraction_free_op>
    : outsize<operations::solve_op> { };

template<>
struct outsize<operations::hilbert_matrix_op>
{
    template<class Expr>
    static slong rows(const Expr& e) {return e._data().first();}
    template<class Expr>
    static slong cols(const Expr& e) {return e._data().second();}
};
} // matrices

// temporary instantiation stuff
FLINTXX_DEFINE_TEMPORARY_RULES(fmpq_matxx)

namespace rules {
FLINT_DEFINE_DOIT_COND2(assignment, FMPQ_MATXX_COND_T, FMPQ_MATXX_COND_S,
        fmpq_mat_set(to._mat(), from._mat()))

FLINTXX_DEFINE_SWAP(fmpq_matxx, fmpq_mat_swap(e1._mat(), e2._mat()))

FLINTXX_DEFINE_EQUALS(fmpq_matxx, fmpq_mat_equal(e1._mat(), e2._mat()))

FLINT_DEFINE_BINARY_EXPR_COND2(times, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_mul(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(times, fmpq_matxx,
        FMPZ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_mul_r_fmpz_mat(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(times, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpq_mat_mul_fmpz_mat(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPZXX_COND_S,
        fmpq_mat_scalar_mul_fmpz(to._mat(), e1._mat(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(divided_by, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPZXX_COND_S,
        fmpq_mat_scalar_div_fmpz(to._mat(), e1._mat(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_direct_op, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_mul_direct(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_cleared_op, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_mul_cleared(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_BINARY_EXPR_COND2(plus, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_add(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(minus, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        fmpq_mat_sub(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(negate, fmpq_matxx, FMPQ_MATXX_COND_S,
        fmpq_mat_neg(to._mat(), from._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(transpose_op, fmpq_matxx, FMPQ_MATXX_COND_S,
        fmpq_mat_transpose(to._mat(), from._mat()))
FLINT_DEFINE_UNARY_EXPR_COND(trace_op, fmpqxx, FMPQ_MATXX_COND_S,
        fmpq_mat_trace(to._fmpq(), from._mat()))

FLINT_DEFINE_THREEARY_EXPR_COND3(mat_at_op, fmpqxx,
        FMPQ_MATXX_COND_S, traits::fits_into_slong, traits::fits_into_slong,
        fmpq_set(to._fmpq(), fmpq_mat_entry(e1._mat(), e2, e3)))

FLINT_DEFINE_BINARY_EXPR_COND2(hilbert_matrix_op, fmpq_matxx,
        traits::fits_into_slong, traits::fits_into_slong,
        to.set_hilbert_matrix())

FLINT_DEFINE_UNARY_EXPR_COND(det_op, fmpqxx, FMPQ_MATXX_COND_S,
        fmpq_mat_det(to._fmpq(), from._mat()))

FLINT_DEFINE_BINARY_EXPR_COND2(solve_fraction_free_op, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        execution_check(fmpq_mat_solve_fraction_free(
                to._mat(), e1._mat(), e2._mat()),
            "solve", "fmpq_mat"))
FLINT_DEFINE_BINARY_EXPR_COND2(solve_dixon_op, fmpq_matxx,
        FMPQ_MATXX_COND_S, FMPQ_MATXX_COND_S,
        execution_check(fmpq_mat_solve_dixon(to._mat(), e1._mat(), e2._mat()),
            "solve", "fmpq_mat"))
FLINT_DEFINE_UNARY_EXPR_COND(inv_op, fmpq_matxx, FMPQ_MATXX_COND_S,
        execution_check(fmpq_mat_inv(to._mat(), from._mat()),
            "inv", "fmpq_mat"))
} // rules
} // flint

#endif
