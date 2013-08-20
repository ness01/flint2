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

    Copyright (C) 2011, 2013 Sebastian Pancratz

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "ulong_extras.h"
#include "long_extras.h"
#include "padic.h"
#include "padic_mat.h"

int main(void)
{
    int i, result;
    flint_rand_t state;

    fmpz_t p;
    slong N;
    padic_ctx_t ctx;
    slong m, n;

    printf("add... ");
    fflush(stdout);

    flint_randinit(state);

    /* Check aliasing: a = a + b */
    for (i = 0; i < 10000; i++)
    {
        padic_mat_t a, b, d;

        fmpz_init_set_ui(p, n_randtest_prime(state, 0));
        N = n_randint(state, PADIC_TEST_PREC_MAX - PADIC_TEST_PREC_MIN) 
            + PADIC_TEST_PREC_MIN;
        padic_ctx_init(ctx, p, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), PADIC_VAL_UNIT);

        m = n_randint(state, 20);
        n = n_randint(state, 20);

        padic_mat_init2(a, m, n, N);
        padic_mat_init2(b, m, n, N);
        padic_mat_init2(d, m, n, N);

        padic_mat_randtest(a, state, ctx);
        padic_mat_randtest(b, state, ctx);

        padic_mat_add(d, a, b, ctx);
        padic_mat_add(a, a, b, ctx);

        result = (padic_mat_equal(a, d) && padic_mat_is_reduced(a, ctx));
        if (!result)
        {
            printf("FAIL:\n\n");
            printf("a = "), padic_mat_print(a, ctx), printf("\n");
            printf("b = "), padic_mat_print(b, ctx), printf("\n");
            printf("d = "), padic_mat_print(d, ctx), printf("\n");
            abort();
        }

        padic_mat_clear(a);
        padic_mat_clear(b);
        padic_mat_clear(d);

        fmpz_clear(p);
        padic_ctx_clear(ctx);
    }

    /* Check aliasing: b = a + b */
    for (i = 0; i < 10000; i++)
    {
        padic_mat_t a, b, d;

        fmpz_init_set_ui(p, n_randtest_prime(state, 0));
        N = n_randint(state, PADIC_TEST_PREC_MAX - PADIC_TEST_PREC_MIN) 
            + PADIC_TEST_PREC_MIN;
        padic_ctx_init(ctx, p, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), PADIC_VAL_UNIT);

        m = n_randint(state, 20);
        n = n_randint(state, 20);

        padic_mat_init2(a, m, n, N);
        padic_mat_init2(b, m, n, N);
        padic_mat_init2(d, m, n, N);

        padic_mat_randtest(a, state, ctx);
        padic_mat_randtest(b, state, ctx);

        padic_mat_add(d, a, b, ctx);
        padic_mat_add(b, a, b, ctx);

        result = (padic_mat_equal(b, d) && padic_mat_is_reduced(b, ctx));
        if (!result)
        {
            printf("FAIL:\n\n");
            printf("a = "), padic_mat_print(a, ctx), printf("\n");
            printf("b = "), padic_mat_print(b, ctx), printf("\n");
            printf("d = "), padic_mat_print(d, ctx), printf("\n");
            abort();
        }

        padic_mat_clear(a);
        padic_mat_clear(b);
        padic_mat_clear(d);

        fmpz_clear(p);
        padic_ctx_clear(ctx);
    }

    /* Check aliasing: a = a + a */
    for (i = 0; i < 10000; i++)
    {
        padic_mat_t a, b, c;

        fmpz_init_set_ui(p, n_randtest_prime(state, 0));
        N = n_randint(state, PADIC_TEST_PREC_MAX - PADIC_TEST_PREC_MIN) 
            + PADIC_TEST_PREC_MIN;
        padic_ctx_init(ctx, p, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), PADIC_VAL_UNIT);

        m = n_randint(state, 20);
        n = n_randint(state, 20);

        padic_mat_init2(a, m, n, N);
        padic_mat_init2(b, m, n, N);
        padic_mat_init2(c, m, n, N);

        padic_mat_randtest(a, state, ctx);
        padic_mat_set(b, a, ctx);

        padic_mat_add(c, b, b, ctx);
        padic_mat_add(b, b, b, ctx);

        result = (padic_mat_equal(b, c) && padic_mat_is_reduced(b, ctx));
        if (!result)
        {
            printf("FAIL (alias b = b + b):\n\n");
            printf("a = "), padic_mat_print(a, ctx), printf("\n");
            printf("b = "), padic_mat_print(b, ctx), printf("\n");
            printf("c = "), padic_mat_print(c, ctx), printf("\n");
            printf("N = %ld\n", N);
            printf("p = "), fmpz_print(p), printf("\n");
            abort();
        }

        padic_mat_clear(a);
        padic_mat_clear(b);
        padic_mat_clear(c);

        fmpz_clear(p);
        padic_ctx_clear(ctx);
    }

    /* Check commutativity: a + b == b + a */
    for (i = 0; i < 10000; i++)
    {
        padic_mat_t a, b, c, d;

        fmpz_init_set_ui(p, n_randtest_prime(state, 0));
        N = n_randint(state, PADIC_TEST_PREC_MAX - PADIC_TEST_PREC_MIN) 
            + PADIC_TEST_PREC_MIN;
        padic_ctx_init(ctx, p, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), PADIC_VAL_UNIT);

        m = n_randint(state, 10);
        n = n_randint(state, 10);

        padic_mat_init2(a, m, n, N);
        padic_mat_init2(b, m, n, N);
        padic_mat_init2(c, m, n, N);
        padic_mat_init2(d, m, n, N);

        padic_mat_randtest(a, state, ctx);
        padic_mat_randtest(b, state, ctx);

        padic_mat_add(c, a, b, ctx);
        padic_mat_add(d, b, a, ctx);

        result = (padic_mat_equal(c, d) && padic_mat_is_reduced(c, ctx));
        if (!result)
        {
            printf("FAIL:\n\n");
            printf("a = "), padic_mat_print_pretty(a, ctx), printf("\n");
            printf("b = "), padic_mat_print_pretty(b, ctx), printf("\n");
            printf("c = "), padic_mat_print_pretty(c, ctx), printf("\n");
            printf("d = "), padic_mat_print_pretty(d, ctx), printf("\n");
            abort();
        }

        padic_mat_clear(a);
        padic_mat_clear(b);
        padic_mat_clear(c);
        padic_mat_clear(d);

        fmpz_clear(p);
        padic_ctx_clear(ctx);
    }

    /* Check zero element: a + 0 == a */
    for (i = 0; i < 10000; i++)
    {
        padic_mat_t a, b;

        fmpz_init_set_ui(p, n_randtest_prime(state, 0));
        N = n_randint(state, PADIC_TEST_PREC_MAX - PADIC_TEST_PREC_MIN) 
            + PADIC_TEST_PREC_MIN;
        padic_ctx_init(ctx, p, FLINT_MAX(0, N-10), FLINT_MAX(0, N+10), PADIC_VAL_UNIT);

        m = n_randint(state, 20);
        n = n_randint(state, 20);

        padic_mat_init2(a, m, n, N);
        padic_mat_init2(b, m, n, N);

        padic_mat_randtest(a, state, ctx);

        padic_mat_add(b, a, b, ctx);

        result = (padic_mat_equal(a, b) && padic_mat_is_canonical(a, ctx));
        if (!result)
        {
            printf("FAIL:\n\n");
            printf("a = "), padic_mat_print(a, ctx), printf("\n");
            printf("b = "), padic_mat_print(b, ctx), printf("\n");
            abort();
        }

        padic_mat_clear(a);
        padic_mat_clear(b);

        fmpz_clear(p);
        padic_ctx_clear(ctx);
    }

    flint_randclear(state);
    _fmpz_cleanup();
    printf("PASS\n");
    return EXIT_SUCCESS;
}

