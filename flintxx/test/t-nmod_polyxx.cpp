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

#include <iostream>
#include <sstream>
#include <string>

#include "nmod_polyxx.h"

#include "flintxx/test/helpers.h"

using namespace flint;

void
test_init()
{
    nmod_polyxx p(10);
    tassert(p.length() == 0);
    tassert(p.modulus() == 10);
}

void
test_manipulation()
{
    mp_limb_t M = 31;
    nmod_polyxx p(M), q(M);
    nmodxx_ctx_srcref ctx = p.estimate_ctx();
    p.set_coeff(5, 17 + M);
    tassert(p.degree() == 5);
    q.set_coeff(5, nmodxx::red(17, ctx));
    tassert((q + nmod_polyxx(M)).get_coeff(5) ==
            nmodxx::red(17, ctx));
    p.set_coeff(0, nmodxx::red(1, ctx));
    tassert(p != q);
    p.set_coeff(0, 0);
    tassert(p == q);

    tassert(p.length() == 6);

    p.realloc(0);
    tassert(p.is_zero() && !p.is_one());
    p.set_coeff(0, 1);
    tassert(p.is_one());
}

void
test_assignment()
{
    mp_limb_t M = 31;
    nmod_polyxx p(M), q(M);
    p.set_coeff(0, 1);
    tassert(p != q);
    p = q;
    tassert(p == q);

    p = "4 31  0 0 0 1";
    q.set_coeff(3, 1);
    tassert(p == q);

    // TODO XXX this does not always fail?
    //assert_exception(p = "2 1 2");
    assert_exception(p = "2  x 2");
}

void
test_conversion()
{
    nmod_polyxx p(31);
    p.set_coeff(3, 1);
    tassert(p.to_string() == "4 31  0 0 0 1");
}

void
test_arithmetic()
{
    mp_limb_t M = 31;
    nmod_polyxx g(M), h(M);
    nmodxx_ctx_srcref ctx = g.estimate_ctx();
    g.set_coeff(0, 17); h.set_coeff(0, 15);
    tassert((g + h).get_coeff(0) == nmodxx::red(15 + 17, ctx));

    frandxx state;
    g.set_randtest(state, 10);
    h.set_randtest(state, 10);

    tassert(((-g) + g).is_zero());
    tassert(g - h == g + (-h));

    tassert(g*nmodxx::red(3, ctx) == g + g + g);
    tassert(g.make_monic() == g*inv(g.get_coeff(g.degree())));

    nmod_polyxx f(M);f.set_coeff(0, 15);
    tassert(f*g == nmodxx::red(15, ctx)*g);
    tassert(h.mul_classical(g) == h.mul_KS(g) && h.mul_KS(g) == h*g);

    f = h*g;f.truncate(7);
    tassert(f == mullow(h, g, 7));
    tassert(f == h.mullow_KS(g, 7));
    tassert(f == h.mullow_classical(g, 7));

    f = (h*g).poly_shift_right(7);
    tassert(f == h.mulhigh(g, 7).poly_shift_right(7));
    tassert(f == h.mulhigh_classical(g, 7).poly_shift_right(7));

    f = h / g;
    tassert(f*g + (h % g) == h);
    tassert(((h*g) % h).is_zero());

    f.set_randtest(state, 10);
    tassert(h.mulmod(g, f) == ((h*g) % f));


    f = "3 31  1 0 1";
    nmodxx x = nmodxx::red(7, ctx);
    tassert(evaluate(f, x) == x*x + nmodxx::red(1, ctx));
    f.realloc(0);f.set_coeff(31, 1);
    tassert(evaluate(f, x) == x);
    tassert(f(x) == x);
}

void
test_functions()
{
    mp_limb_t M = 31;
    nmod_polyxx g(M);
    //nmodxx_ctx_srcref ctx = g.estimate_ctx();

    g.set_coeff(5, 15);
    tassert(g.max_bits() == 4);

    g.truncate(3);
    tassert(g.is_zero());

    g.set_coeff(15, 1);
    tassert(g.poly_shift_right(15).is_one());
    tassert(g.poly_shift_right(15).poly_shift_left(15) == g);

    frandxx rand;
    g.set_randtest(rand, 15);
    tassert(g.length() <= 15);
    g.set_randtest_irreducible(rand, 15);
    tassert(g.length() <= 15);
    tassert(g.is_squarefree());
    tassert(g.is_irreducible());

    tassert(g == poly_bit_unpack(g.bit_pack(5u), 5u));

    // multiplication, division, modulo tested in arithmetic

    tassert(g.pow(3u) == g*g*g);
    tassert(g.pow(5u) == g.pow_binexp(5u));

    nmod_polyxx res(g.pow(15u));res.truncate(12);
    tassert(res == g.pow_trunc(15u, 12));
    tassert(res == g.pow_trunc_binexp(15u, 12));

    nmod_polyxx f(M);f.set_randtest(rand, 10);
    res = g.pow(10u) % f;
    tassert(res == g.powmod_binexp(10u, f));

    res = "5 31  1 1 1 1 1";
    tassert(res.derivative().to_string() == "4 31  1 2 3 4");
    tassert(g.integral().derivative() == g);
}

// test stuff which we should get automatically - addmul, references etc
void
test_extras()
{
    // TODO
}

int
main()
{
    std::cout << "nmod_polyxx....";

    test_init();
    test_manipulation();
    test_assignment();
    test_conversion();
    test_arithmetic();
    test_functions();
    test_extras();

    std::cout << "PASS" << std::endl;
    return 0;
}
