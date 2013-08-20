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

/*
    Benchmarks for the q-adic square root routine.

    We consider the set-up with p = 17, N = 2^i, i = 0, ..., 19, 
    and compute the square root of B = A^2, A mod p^N, where 

        A = [a{0},...,a{d-1}], where a{i} = (3+i)^{3N}.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <mpir.h>

#include "flint.h"
#include "fmpz.h"
#include "qadic.h"

int
main(void)
{
    slong l, len = 20;
    slong runs[] = {
        10, 10, 10, 10, 10, 
        10, 10, 10, 10, 10, 
        10, 10, 10, 1, 1, 
        1, 1, 1, 1, 1
    };
    slong N[] = {
        1, 2, 4, 8, 16, 
        32, 64, 128, 256, 512, 
        1024, 1L << 11, 1L << 12, 1L << 13, 1L << 14, 
        1L << 15, 1L << 16, 1L << 17, 1L << 18, 1L << 19
    };
    slong T[20] = {0};

    printf("Benchmark for q-adic square root.\n");
    fflush(stdout);

for (l = 0; l < len; l++)
{
    flint_rand_t state;
    slong d = 97, i, n = N[l], r;
    clock_t c0, c1;
    long doublecputime;

    fmpz_t p;
    qadic_ctx_t ctx;
    qadic_t a, b, z;

    flint_randinit(state);

    fmpz_init_set_ui(p, 17);

    qadic_ctx_init_conway(ctx, p, d, n, "X", PADIC_VAL_UNIT);

    qadic_init(a);
    qadic_init(b);
    qadic_init(z);

    padic_poly_fit_length(a, d);
    _padic_poly_set_length(a, d);
    a->val = 0;

    for (i = 0; i < d; i++)
    {
        fmpz_t f, pow;

        fmpz_init(f);
        fmpz_init(pow);
        fmpz_set_ui(f, 3 + i);
        fmpz_pow_ui(pow, p, n);
        fmpz_pow_ui(a->coeffs + i, f, 3 * n);
        fmpz_mod(a->coeffs + i, a->coeffs + i, pow);
        fmpz_clear(f);
        fmpz_clear(pow);
    }
    _padic_poly_normalise(a);
    qadic_mul(b, a, a, ctx);

    c0 = clock();
    for (r = runs[l]; (r); r--)
    {
        qadic_sqrt(z, b, ctx);
        qadic_zero(z);
    }
    c1 = clock();

    qadic_sqrt(z, b, ctx);
    qadic_mul(z, z, z, ctx);
    if (!qadic_equal(b, z))
    {
        printf("FAIL:\n");
        abort();
    }

    cputime = (long double) (c1 - c0) / (long double) CLOCKS_PER_SEC;

    T[l] = (slong) (cputime * (1000000000 / runs[l]));

    printf("%2ld, %4LG, %8ld, %ld\n", 
        l, cputime, runs[l], T[l]);

    qadic_clear(a);
    qadic_clear(b);
    qadic_clear(z);

    fmpz_clear(p);
    qadic_ctx_clear(ctx);
    flint_randclear(state);
}

    printf("Output as a list:\n");
    for (l = 0; l < len; l++)
        printf("%ld, ", T[l]);
    printf("\n");
}

