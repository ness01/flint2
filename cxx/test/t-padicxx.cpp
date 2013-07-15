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

#include "cxx/padicxx.h"
#include "cxx/test/helpers.h"

using namespace flint;

void
test_init()
{
    padicxx_ctx ctx(fmpzxx(5), 10, 20, PADIC_TERSE);
    tassert(ctx.get_p() == 5);

    padicxx a(ctx, 20);
    tassert(&a.get_ctx() == &ctx);
    tassert(a.prec() == 20);

    padicxx c(a);
    tassert(a == c);

    padicxx b(ctx, 30);
    tassert(&(a + b).estimate_ctx() == &ctx);
    tassert((a + b).prec() == 30);

    tassert((a + b).create_temporary()._prec() == 30);
    padicxx d((a + b).create_temporary());
    tassert(&d.get_ctx() == &ctx);
    tassert(d.prec() == 30);

    padicxx e(a + b);
    tassert(e.prec() == 30);
}

void
test_assignment()
{
    padicxx_ctx ctx(fmpzxx(5), 10, 20, PADIC_TERSE);
    padicxx a(ctx, 20), b(ctx, 20);
    fmpzxx c(17); fmpqxx d(17, 1u);

    a = 17; tassert(a != b);
    b = 17; tassert(a == b);
    b = 0; tassert(a != b);
    b = 17ul; tassert(a == b);
    b = 0; b = c; tassert(a == b);
    b = 0; b = fmpzxx_ref(c); tassert(a == b);
    b = 0; b = fmpzxx_srcref(c); tassert(a == b);
    b = 0; b = d; tassert(a == b);
}

void
test_conversion()
{
    padicxx_ctx ctx(fmpzxx(5), 10, 20, PADIC_TERSE);
    padicxx_ctx ctx2(fmpzxx(5), 10, 20, PADIC_VAL_UNIT);

    padicxx a(ctx), b(ctx2);
    a = 15; b = 15;
    tassert(a.to_string() == "15");
    tassert(b.to_string() == "3*5");

    tassert(a.to<fmpzxx>() == 15);
    tassert(a.to<fmpqxx>() == fmpqxx(15, 1u));

    std::ostringstream oss;
    oss << a << ' ' << b;
    tassert(oss.str() == "15 3*5");
}

template<class T>
padicxx make_padic(const T& t, long prec = PADIC_DEFAULT_PREC)
{
    static padicxx_ctx ctx(fmpzxx(5), 10, 20, PADIC_TERSE);
    padicxx p(ctx);
    p = t;
    return p;
}
template<class T, class U>
bool fuzzy_equals(const T& t, const U& u)
{
    long prec = std::min(t.prec(), u.prec()) - 5;
    padicxx a(t.estimate_ctx(), prec); a = t;
    padicxx b(t.estimate_ctx(), prec); b = u;
    return a == b;
}
void
test_arithmetic()
{
    fmpqxx af(17, 25u), bf(5, 27u);
    padicxx a(make_padic(af));
    padicxx b(make_padic(bf));

    tassert(fuzzy_equals(a + b, make_padic(af + bf)));
    tassert(fuzzy_equals(a * b, make_padic(af * bf)));
    tassert(fuzzy_equals(a / b, make_padic(af / bf)));
    tassert(fuzzy_equals(a - b, make_padic(af - bf)));
    tassert(fuzzy_equals(a << 2, make_padic(af * fmpzxx(25))));
    tassert(fuzzy_equals(a >> 2, make_padic(af / fmpzxx(25))));
    tassert(-a == make_padic(-af));
}

void
test_functions()
{
    padicxx a(make_padic(fmpqxx(14, 25u)));
    tassert(fuzzy_equals(a, pow(sqrt(a), 2)));

    bool exception_occured = false;
    try
    {
        sqrt(make_padic(fmpqxx(2, 1u))).evaluate();
    }
    catch(const padicxx_exception&)
    {
        exception_occured = true;
    }
    tassert(exception_occured);

    fmpqxx cf(14*5, 1u);
    padicxx c = make_padic(fmpqxx(14*5, 1u));
    tassert(fuzzy_equals(log(exp(c)), c));
}

int
main()
{
    std::cout << "padicxx....";

    test_init();
    test_assignment();
    test_conversion();
    test_arithmetic();
    test_functions();

    std::cout << "PASS" << std::endl;
    return 0;
}
