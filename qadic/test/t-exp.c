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

    Copyright (C) 2012 Sebastian Pancratz

******************************************************************************/

#include <stdio.h>

#include "qadic.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, result;
    flint_rand_t state;

    printf("exp... ");
    fflush(stdout);

    flint_randinit(state);

    /* Check aliasing */
    for (i = 0; i < 1000; i++)
    {
        fmpz_t p;
        slong d, N;
        qadic_ctx_t ctx;

        qadic_t a, b, c;
        int ans1, ans2;

        fmpz_init_set_ui(p, n_randprime(state, 2 + n_randint(state, 3), 1));
        d = n_randint(state, 10) + 1;
        N = n_randint(state, 50) + 1;
        qadic_ctx_init_conway(ctx, p, d, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), "a", PADIC_SERIES);

        qadic_init2(a, N);
        qadic_init2(b, N);
        qadic_init2(c, N);

        qadic_randtest(a, state, ctx);
        qadic_set(b, a, ctx);

        ans1 = qadic_exp(c, b, ctx);
        ans2 = qadic_exp(b, b, ctx);

        result = ((ans1 == ans2) && (!ans1 || qadic_equal(b, c)));
        if (!result)
        {
            printf("FAIL (alias):\n\n");
            printf("a = "), qadic_print_pretty(a, ctx), printf("\n");
            printf("b = "), qadic_print_pretty(b, ctx), printf("\n");
            printf("c = "), qadic_print_pretty(c, ctx), printf("\n");
            printf("ans1 = %d\n", ans1);
            printf("ans2 = %d\n", ans2);
            abort();
        }

        qadic_clear(a);
        qadic_clear(b);
        qadic_clear(c);

        fmpz_clear(p);
        qadic_ctx_clear(ctx);
    }

    /* Functional equation: exp(a + b) == exp(a) exp(b) */
    for (i = 0; i < 1000; i++)
    {
        fmpz_t p;
        slong deg, N;
        qadic_ctx_t ctx;

        qadic_t a, b, c, d, e, f, g;
        int ans1, ans2, ans3;

        fmpz_init_set_ui(p, n_randprime(state, 2 + n_randint(state, 3), 1));
        deg = n_randint(state, 10) + 1;
        N   = n_randint(state, 50) + 1;
        qadic_ctx_init_conway(ctx, p, deg, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), "a", PADIC_SERIES);

        qadic_init2(a, N);
        qadic_init2(b, N);
        qadic_init2(c, N);
        qadic_init2(d, N);
        qadic_init2(e, N);
        qadic_init2(f, N);
        qadic_init2(g, N);

        qadic_randtest(a, state, ctx);
        qadic_randtest(b, state, ctx);
        qadic_add(c, a, b, ctx);

        ans1 = qadic_exp(d, a, ctx);
        ans2 = qadic_exp(e, b, ctx);
        qadic_mul(f, d, e, ctx);

        ans3 = qadic_exp(g, c, ctx);

        result = (!ans1 || !ans2 || (ans3 && qadic_equal(f, g)));
        if (!result)
        {
            printf("FAIL (functional equation):\n\n");
            printf("a                 = "), qadic_print_pretty(a, ctx), printf("\n");
            printf("b                 = "), qadic_print_pretty(b, ctx), printf("\n");
            printf("c = a + b         = "), qadic_print_pretty(c, ctx), printf("\n");
            printf("d = exp(a)        = "), qadic_print_pretty(d, ctx), printf("\n");
            printf("e = exp(b)        = "), qadic_print_pretty(e, ctx), printf("\n");
            printf("f = exp(a) exp(b) = "), qadic_print_pretty(f, ctx), printf("\n");
            printf("g = exp(a + b)    = "), qadic_print_pretty(g, ctx), printf("\n");
            abort();
        }

        qadic_clear(a);
        qadic_clear(b);
        qadic_clear(c);
        qadic_clear(d);
        qadic_clear(e);
        qadic_clear(f);
        qadic_clear(g);

        fmpz_clear(p);
        qadic_ctx_clear(ctx);
    }

    flint_randclear(state);
    _fmpz_cleanup();
    printf("PASS\n");
    return EXIT_SUCCESS;
}

